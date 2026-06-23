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
    /* TODO 1: Create a bbox view for video channel 1. */
    /* TODO 2: Clear old boxes from the view. */
    /* TODO 3: Configure outline style, thin thickness, and red color. */
    /* TODO 4: Draw and commit a rectangle. */
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
    GMainLoop* loop = NULL;

    openlog(NULL, LOG_PID, LOG_USER);

    loop = g_main_loop_new(NULL, FALSE);
    g_unix_signal_add(SIGTERM, signal_handler, loop);
    g_unix_signal_add(SIGINT, signal_handler, loop);

    single_channel();

    g_main_loop_run(loop);

    clear();

    return EXIT_SUCCESS;
}
