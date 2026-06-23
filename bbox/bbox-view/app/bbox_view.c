#include <bbox.h>
#include <glib-unix.h>
#include <glib.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>


__attribute__((noreturn)) __attribute__((format(printf, 1, 2))) static void
panic(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsyslog(LOG_ERR, format, args);
    va_end(args);

    exit(EXIT_FAILURE);
}


static void single_channel(void) {
    // Draw on a single view: 1
    bbox_t* bbox = bbox_view_new(1u);
    if (!bbox)
        panic("Failed creating: %s", strerror(errno));

    
    //bbox_coordinates_frame_normalized(bbox);

    bbox_clear(bbox);  // Remove all old bounding-boxes

    // Create all needed colors [These operations are slow!]
    const bbox_color_t red   = bbox_color_from_rgb(0xff, 0x00, 0x00);
    //const bbox_color_t blue  = bbox_color_from_rgb(0x00, 0x00, 0xff);
    //const bbox_color_t green = bbox_color_from_rgb(0x00, 0xff, 0x00);

    bbox_style_outline(bbox);                      
    bbox_thickness_thin(bbox);                     
    bbox_color(bbox, red);                         
    bbox_rectangle(bbox, 0.05, 0.05, 0.95, 0.95);  

    // Draw all queued geometry simultaneously
    if (!bbox_commit(bbox, 0u))
        panic("Failed committing: %s", strerror(errno));

}


static void clear(void) {
    // Draw on a single channel: 1
    bbox_t* bbox = bbox_new(1u, 1u);
    if (!bbox)
        panic("Failed creating: %s", strerror(errno));

    bbox_clear(bbox);  // Remove all old bounding-boxes

    // Clear everything simultaneously
    if (!bbox_commit(bbox, 0u))
        panic("Failed committing: %s", strerror(errno));

    
    bbox_destroy(bbox);
}
static gboolean signal_handler(gpointer loop) {
    g_main_loop_quit((GMainLoop*)loop);
    clear();
    syslog(LOG_INFO, "Application was stopped by SIGTERM or SIGINT.");

    return G_SOURCE_REMOVE;
}

int main(void) {
    /* TODO 1: Review the README steps for manifest and Makefile changes. */
    /* TODO 2: Paste the setup snippet into this main function. */
    /* TODO 3: Paste the runtime/API workflow snippets in order. */
    /* TODO 4: Paste the cleanup snippet at the end. */

    return 0;
}
