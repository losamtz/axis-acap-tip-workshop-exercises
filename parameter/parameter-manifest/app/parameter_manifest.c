/*
 * Parameter manifest exercise skeleton.
 *
 * The signal handler and basic includes are provided. Follow README.md to add
 * the manifest parameter, Makefile dependency, parameter callback, AXParameter
 * handle setup, callback registration, main loop, and cleanup.
 */

#include <axsdk/axparameter.h>
#include <glib-unix.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#define APP_NAME "parameter_manifest"

static gboolean signal_handler(gpointer loop) {
    g_main_loop_quit((GMainLoop*)loop);
    syslog(LOG_INFO, "Application was stopped by SIGTERM or SIGINT.");
    return G_SOURCE_REMOVE;
}

/* TODO 3: Paste the parameter callback from README.md here. */

int main(void) {
    GError* error = NULL;
    GMainLoop* loop = NULL;

    openlog(APP_NAME, LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Starting %s", APP_NAME);

    /* TODO 1: Add the ParameterManifest entry to manifest.json. */
    /* TODO 2: Add axparameter to the Makefile PKGS line. */

    /* TODO 4: Create the AXParameter handle here. */

    /* TODO 5: Register the ParameterManifest callback here. */

    /* TODO 6: Create the GLib main loop, register signals, run, then clean up. */

    closelog();
    return EXIT_SUCCESS;
}
