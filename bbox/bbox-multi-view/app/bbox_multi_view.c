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


static double xpos = 0.0;  // starting x position
static const double box_width = 0.1;
static const double y = 0.3;
static const double height = 0.1;
static int dir = -1;



__attribute__((noreturn)) __attribute__((format(printf, 1, 2))) static void
panic(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsyslog(LOG_ERR, format, args);
    va_end(args);

    exit(EXIT_FAILURE);
}


static gboolean update_bbox(gpointer user_data) {
    (void)user_data;

    /* TODO 1: Create a bbox handle for views 1, 2, 3, and 4. */
    /* TODO 2: Enable video output and clear previous boxes. */
    /* TODO 3: Configure yellow corner-style boxes. */
    /* TODO 4: Update the horizontal animation position. */
    /* TODO 5: Draw, commit, pause, and destroy this frame's bbox handle. */
    
    return G_SOURCE_CONTINUE;  // keep the timer running
}

static void clear(void) {
    bbox_t* bbox = bbox_new(4u, 1u, 2u, 3u, 4u);
    if (!bbox)
        panic("Failed creating: %s", strerror(errno));

    bbox_clear(bbox);  // Remove all old bounding-boxes

    // Clear everything simultaneously
    if (!bbox_commit(bbox, 0u))
        panic("Failed committing: %s", strerror(errno));

    
    sleep(1);
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

    openlog(NULL, LOG_PID | LOG_CONS, LOG_USER);

    loop = g_main_loop_new(NULL, FALSE);
    g_unix_signal_add(SIGTERM, signal_handler, loop);
    g_unix_signal_add(SIGINT, signal_handler, loop);

    g_timeout_add(100, update_bbox, NULL);

    g_main_loop_run(loop);

    clear();

    return EXIT_SUCCESS;
}
