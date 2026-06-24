// Copyright (C) 2026 Axis Communications AB, Lund, Sweden
// Licensed under the MIT License. See LICENSE file for details.

#include <assert.h>
#include <axoverlay2.h>
#include <cairo/cairo.h>
#include <gio/gio.h>
#include <glib-unix.h>
#include <glib.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <vdo-error.h>
#include <vdo-stream.h>

struct overlay {
    int overlay_id;
    unsigned stream_id;
    unsigned used_width;
    unsigned used_height;
    unsigned full_width;
    unsigned full_height;
    cairo_surface_t* surface;
};

static void overlay_record_deleter(void* overlay_void);
static gboolean signal_callback(gpointer userdata);
static gboolean animation_tick_callback(gpointer userdata);
static gboolean stream_event_callback(GIOChannel* channel, GIOCondition condition, gpointer userdata);
static void create_overlay(unsigned stream_id, unsigned stream_width, unsigned stream_height);
static void remove_overlay(unsigned stream_id);
static void process_next_frame(struct overlay* overlay);
static void render_frame(struct overlay* overlay, char* target_buffer);

static VdoStream* vdo_event_stream = NULL;
static GHashTable* overlay_table = NULL;
static GMainLoop* main_loop = NULL;
static const unsigned tick_period_us = 1000000;
static unsigned animation_state = 0;

int main(void) {
    /* TODO 1: Initialize errors, logging, cache path, axoverlay2, storage, and loop. */
    /* TODO 2: Open VDO stream 0 and attach the overlay stream-event filter. */
    /* TODO 3: Add the VDO event fd and shutdown signals to the GLib main loop. */
    /* TODO 4: Run the loop so stream events create overlays and the timer updates text. */
    /* TODO 5: Stop axoverlay2 and release VDO, GLib, and overlay resources. */

    return 0;
}

static void overlay_record_deleter(void* overlay_void) {
    struct overlay* overlay = overlay_void;
    if (!overlay)
        return;
    if (overlay->surface)
        cairo_surface_destroy(overlay->surface);
    g_free(overlay);
}

static gboolean signal_callback(gpointer userdata) {
    (void)userdata;
    if (main_loop)
        g_main_loop_quit(main_loop);
    return G_SOURCE_REMOVE;
}

static gboolean animation_tick_callback(gpointer userdata) {
    (void)userdata;

    GHashTableIter iter;
    gpointer key = NULL;
    gpointer value = NULL;

    animation_state++;

    g_hash_table_iter_init(&iter, overlay_table);
    while (g_hash_table_iter_next(&iter, &key, &value))
        process_next_frame(value);

    return G_SOURCE_CONTINUE;
}

static gboolean stream_event_callback(GIOChannel* channel,
                                      GIOCondition condition,
                                      gpointer userdata) {
    (void)channel;
    (void)userdata;

    GError* error = NULL;
    VdoMap* vdo_event = NULL;
    VdoStream* vdo_stream = NULL;
    VdoMap* stream_info = NULL;
    gboolean ret = G_SOURCE_CONTINUE;

    if (condition & (G_IO_ERR | G_IO_HUP)) {
        syslog(LOG_ERR, "Connection to VDO was broken, condition=0x%04x", condition);
        g_main_loop_quit(main_loop);
        return G_SOURCE_REMOVE;
    }

    vdo_event = vdo_stream_get_event(vdo_event_stream, &error);
    if (!vdo_event) {
        if (g_error_matches(error, VDO_ERROR, VDO_ERROR_NO_EVENT))
            goto out;
        syslog(LOG_ERR, "Failed to get VDO stream event: %s", error->message);
        g_main_loop_quit(main_loop);
        ret = G_SOURCE_REMOVE;
        goto out;
    }

    unsigned event_type = vdo_map_get_uint32(vdo_event, "event", 0);
    unsigned stream_id = vdo_map_get_uint32(vdo_event, "id", 0);

    if (event_type == VDO_STREAM_EVENT_EXISTING || event_type == VDO_STREAM_EVENT_CREATED) {
        vdo_stream = vdo_stream_get(stream_id, &error);
        if (!vdo_stream) {
            syslog(LOG_ERR, "Failed to get VDO stream %u: %s", stream_id, error->message);
            g_main_loop_quit(main_loop);
            ret = G_SOURCE_REMOVE;
            goto out;
        }

        stream_info = vdo_stream_get_info(vdo_stream, NULL);
        if (!stream_info) {
            syslog(LOG_ERR, "VDO stream %u is missing stream info", stream_id);
            g_main_loop_quit(main_loop);
            ret = G_SOURCE_REMOVE;
            goto out;
        }

        unsigned width = vdo_map_get_uint32(stream_info, "width", 0);
        unsigned height = vdo_map_get_uint32(stream_info, "height", 0);
        if (!width || !height) {
            syslog(LOG_ERR, "VDO stream %u has invalid size %ux%u", stream_id, width, height);
            g_main_loop_quit(main_loop);
            ret = G_SOURCE_REMOVE;
            goto out;
        }

        create_overlay(stream_id, width, height);
    } else if (event_type == VDO_STREAM_EVENT_CLOSED) {
        remove_overlay(stream_id);
    }

out:
    g_clear_error(&error);
    if (vdo_event)
        g_object_unref(vdo_event);
    if (vdo_stream)
        g_object_unref(vdo_stream);
    if (stream_info)
        g_object_unref(stream_info);
    return ret;
}

static void create_overlay(unsigned stream_id, unsigned stream_width, unsigned stream_height) {
    axo_err* axo_error = NULL;
    axo_props* props = NULL;
    axo_match* match = NULL;

    bool use_upscale = stream_width * stream_height > 4000000;
    unsigned used_width = use_upscale ? stream_width / 2 : stream_width;
    unsigned used_height = use_upscale ? stream_height / 2 : stream_height;
    unsigned full_width = 0;
    unsigned full_height = 0;

    if (!axo_get_aligned_size(AXO_FORMAT_ARGB32,
                              used_width,
                              used_height,
                              &full_width,
                              &full_height,
                              &axo_error)) {
        syslog(LOG_ERR, "Failed to align overlay size: %s", axo_err_get_message(axo_error));
        goto out;
    }

    props = axo_props_new();
    axo_props_set_format(props, AXO_FORMAT_ARGB32);
    axo_props_set_size(props, full_width, full_height);
    axo_props_set_upscale_x2(props, use_upscale);

    match = axo_match_new();
    axo_match_stream_id(match, stream_id);

    int overlay_id = axo_create_overlay(props, match, &axo_error);
    if (overlay_id < 0) {
        if (axo_err_get_code(axo_error) != AXO_ERR_NO_STREAM)
            syslog(LOG_ERR, "Failed to create overlay on stream %u: %s",
                   stream_id, axo_err_get_message(axo_error));
        goto out;
    }

    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                         (int)full_width,
                                                         (int)full_height);
    assert(full_width * sizeof(uint32_t) == (unsigned)cairo_image_surface_get_stride(surface));

    struct overlay* overlay = g_malloc(sizeof(*overlay));
    *overlay = (struct overlay){
        .overlay_id = overlay_id,
        .stream_id = stream_id,
        .used_width = used_width,
        .used_height = used_height,
        .full_width = full_width,
        .full_height = full_height,
        .surface = surface,
    };

    g_hash_table_insert(overlay_table, GUINT_TO_POINTER(stream_id), overlay);
    syslog(LOG_INFO, "Created overlay %d on stream %u", overlay_id, stream_id);
    process_next_frame(overlay);

out:
    axo_err_clear(&axo_error);
    if (props)
        axo_props_free(props);
    if (match)
        axo_match_free(match);
}

static void remove_overlay(unsigned stream_id) {
    axo_err* axo_error = NULL;
    const struct overlay* overlay = g_hash_table_lookup(overlay_table, GUINT_TO_POINTER(stream_id));

    if (!overlay)
        goto out;

    if (!axo_remove_overlay(overlay->overlay_id, &axo_error)) {
        syslog(LOG_ERR, "Failed to remove overlay %d on stream %u: %s",
               overlay->overlay_id, stream_id, axo_err_get_message(axo_error));
    }

out:
    g_hash_table_remove(overlay_table, GUINT_TO_POINTER(stream_id));
    axo_err_clear(&axo_error);
}

static void process_next_frame(struct overlay* overlay) {
    axo_err* axo_error = NULL;
    axo_buffer* buffer = axo_get_buffer(overlay->overlay_id, NULL, &axo_error);
    if (!buffer) {
        axo_err_code code = axo_err_get_code(axo_error);
        if (code != AXO_ERR_NO_STREAM && code != AXO_ERR_WAIT)
            syslog(LOG_ERR, "Failed to get buffer for overlay %d: %s",
                   overlay->overlay_id, axo_err_get_message(axo_error));
        goto out;
    }

    char* target_buffer = axo_buffer_get_data(buffer, &axo_error);
    if (!target_buffer) {
        syslog(LOG_ERR, "Failed to get overlay buffer data: %s", axo_err_get_message(axo_error));
        goto out;
    }

    render_frame(overlay, target_buffer);

    if (!axo_submit_buffer(buffer, NULL, &axo_error)) {
        syslog(LOG_ERR, "Failed to submit overlay buffer %d: %s",
               overlay->overlay_id, axo_err_get_message(axo_error));
        goto out;
    }

out:
    axo_err_clear(&axo_error);
}

static void render_frame(struct overlay* overlay, char* target_buffer) {
    cairo_t* cr = cairo_create(overlay->surface);

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    int counter = 10 - (int)(animation_state % 11);
    double r = counter <= 3 ? 1.0 : 0.0;
    double g = counter > 7 ? 0.75 : 0.0;
    double b = counter > 3 && counter <= 7 ? 1.0 : 0.0;
    char text[64];
    snprintf(text, sizeof(text), "Countdown %d", counter);

    cairo_select_font_face(cr, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, (double)overlay->used_height * 0.06);
    cairo_set_source_rgb(cr, r, g, b);

    cairo_text_extents_t extents;
    cairo_text_extents(cr, text, &extents);
    cairo_move_to(cr,
                  ((double)overlay->used_width - extents.width) / 2.0 - extents.x_bearing,
                  ((double)overlay->used_height - extents.height) / 2.0 - extents.y_bearing);
    cairo_show_text(cr, text);

    cairo_destroy(cr);
    cairo_surface_flush(overlay->surface);

    unsigned byte_size = overlay->full_width * overlay->full_height * sizeof(uint32_t);
    memcpy(target_buffer, cairo_image_surface_get_data(overlay->surface), byte_size);
}
