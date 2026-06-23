// Copyright (C) 2026 Axis Communications AB, Lund, Sweden
// Licensed under the MIT License. See LICENSE file for details.

#include <assert.h>
#include <axoverlay2.h>
#include <cairo/cairo.h>
#include <gio/gio.h>
#include <glib-unix.h>
#include <glib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <syslog.h>
#include <vdo-error.h>
#include <vdo-stream.h>

struct overlay {
    int overlay_id;
    unsigned stream_id;
    unsigned view_id;
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
static void create_overlay(unsigned stream_id,
                           unsigned stream_width,
                           unsigned stream_height,
                           unsigned view_id);
static void remove_overlay(unsigned stream_id);
static void process_next_frame(struct overlay* overlay);
static void render_frame(struct overlay* overlay, char* target_buffer);

static VdoStream* vdo_event_stream = NULL;
static GHashTable* overlay_table = NULL;
static GMainLoop* main_loop = NULL;
static const unsigned tick_period_us = 1000000 / 15;

int main(void) {
    /* TODO 1: Review the README steps for manifest and Makefile changes. */
    /* TODO 2: Paste the setup snippet into this main function. */
    /* TODO 3: Paste the runtime/API workflow snippets in order. */
    /* TODO 4: Paste the cleanup snippet at the end. */

    return 0;
}
