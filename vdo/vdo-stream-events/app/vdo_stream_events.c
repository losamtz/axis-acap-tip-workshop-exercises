/*
 * Copyright 2026 Axis Communications AB, Lund, Sweden
 *
 * Licensed under the Apache License, Version 2.0.
 */

#include <gio/gio.h>
#include <glib-unix.h>
#include <glib.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <syslog.h>
#include <vdo-error.h>
#include <vdo-stream.h>

static GMainLoop* main_loop = NULL;
static VdoStream* event_stream = NULL;

__attribute__((noreturn)) __attribute__((format(printf, 1, 2))) static void
panic(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsyslog(LOG_ERR, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

static const char* event_name(unsigned event_type) {
    switch (event_type) {
        case VDO_STREAM_EVENT_EXISTING:
            return "EXISTING";
        case VDO_STREAM_EVENT_CREATED:
            return "CREATED";
        case VDO_STREAM_EVENT_CLOSED:
            return "CLOSED";
        default:
            return "UNKNOWN";
    }
}

static void log_stream_info(unsigned stream_id) {
    GError* error = NULL;
    VdoStream* stream = vdo_stream_get(stream_id, &error);
    if (!stream) {
        syslog(LOG_WARNING, "Could not open stream %u for info: %s", stream_id, error->message);
        g_clear_error(&error);
        return;
    }

    VdoMap* info = vdo_stream_get_info(stream, NULL);
    if (!info) {
        syslog(LOG_WARNING, "Stream %u did not provide stream info", stream_id);
        g_object_unref(stream);
        return;
    }

    unsigned width = vdo_map_get_uint32(info, "width", 0);
    unsigned height = vdo_map_get_uint32(info, "height", 0);
    unsigned format = vdo_map_get_uint32(info, "format", 0);
    unsigned camera = vdo_map_get_uint32(info, "camera", 0);
    unsigned rotation = vdo_map_get_uint32(info, "rotation", 0);

    syslog(LOG_INFO,
           "Stream %u info: width=%u height=%u format=%u camera=%u rotation=%u",
           stream_id,
           width,
           height,
           format,
           camera,
           rotation);

    g_object_unref(info);
    g_object_unref(stream);
}

static gboolean signal_handler(gpointer user_data) {
    (void)user_data;
    g_main_loop_quit(main_loop);
    return G_SOURCE_REMOVE;
}

static gboolean stream_event_callback(GIOChannel* channel,
                                      GIOCondition condition,
                                      gpointer user_data) {
    (void)channel;
    (void)user_data;

    GError* error = NULL;
    VdoMap* event = NULL;

    if (condition & (G_IO_ERR | G_IO_HUP)) {
        syslog(LOG_ERR, "VDO event connection was closed, condition=0x%04x", condition);
        g_main_loop_quit(main_loop);
        return G_SOURCE_REMOVE;
    }

    event = vdo_stream_get_event(event_stream, &error);
    if (!event) {
        if (g_error_matches(error, VDO_ERROR, VDO_ERROR_NO_EVENT)) {
            g_clear_error(&error);
            return G_SOURCE_CONTINUE;
        }

        syslog(LOG_ERR, "Failed to read VDO stream event: %s", error->message);
        g_clear_error(&error);
        g_main_loop_quit(main_loop);
        return G_SOURCE_REMOVE;
    }

    unsigned event_type = vdo_map_get_uint32(event, "event", 0);
    unsigned stream_id = vdo_map_get_uint32(event, "id", 0);

    syslog(LOG_INFO, "VDO stream event: type=%s id=%u", event_name(event_type), stream_id);

    if (event_type == VDO_STREAM_EVENT_EXISTING || event_type == VDO_STREAM_EVENT_CREATED)
        log_stream_info(stream_id);

    g_object_unref(event);
    return G_SOURCE_CONTINUE;
}

int main(void) {
    /* TODO 1: Initialize logging, errors, GLib main loop, and signal handlers. */
    /* TODO 2: Open VDO stream 0 and attach an overlay stream-event filter. */
    /* TODO 3: Get the VDO event fd and add it to the GLib main loop. */
    /* TODO 4: Run the main loop so stream_event_callback() handles events. */
    /* TODO 5: Remove the watch, release VDO/GLib objects, and close logging. */

    return 0;
}
