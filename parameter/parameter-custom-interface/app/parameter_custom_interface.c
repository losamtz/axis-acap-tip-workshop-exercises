/*
 * Parameter custom interface exercise skeleton.
 *
 * The signal handler, delayed setter, and callbacks are provided. Follow
 * README.md to add the manifest configuration, Makefile dependency,
 * AXParameter handle setup, callback registration, main loop, and cleanup.
 */

#include <axsdk/axparameter.h>
#include <glib-unix.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include "panic.h"

static GMainLoop* main_loop = NULL;
static AXParameter* axparameter = NULL;

struct message {
    char* name;
    char* value;
};

static gboolean signal_handler(gpointer loop) {
    g_main_loop_quit((GMainLoop*)loop);
    syslog(LOG_INFO, "Application was stopped by SIGTERM or SIGINT.");
    return G_SOURCE_REMOVE;
}

static gboolean set_parameter(void* msg_ptr) {
    GError* error = NULL;
    struct message* msg = msg_ptr;

    if (!ax_parameter_set(axparameter, msg->name, msg->value, TRUE, &error)) {
        panic("%s", error->message);
    }

    syslog(LOG_INFO, "[set-param] Parameter '%s' set to '%s'", msg->name, msg->value);

    free(msg->name);
    free(msg->value);
    free(msg);
    return G_SOURCE_REMOVE;
}

static void multicast_address_callback(const gchar* name,
                                       const gchar* value,
                                       gpointer user_data) {
    (void)user_data;
    if (!value) {
        syslog(LOG_ERR, "Unexpected NULL value for %s", name);
        return;
    }

    struct message* msg = malloc(sizeof(struct message));
    msg->name = strdup("root.Network.RTP.R0.VideoAddress");
    msg->value = strdup(value);
    g_timeout_add_seconds(1, set_parameter, msg);

    syslog(LOG_INFO, "MulticastAddress changed to '%s'", value);
}

static void multicast_port_callback(const gchar* name,
                                    const gchar* value,
                                    gpointer user_data) {
    (void)user_data;
    if (!value) {
        syslog(LOG_ERR, "Unexpected NULL value for %s", name);
        return;
    }

    struct message* msg = malloc(sizeof(struct message));
    msg->name = strdup("root.Network.RTP.R0.VideoPort");
    msg->value = strdup(value);
    g_timeout_add_seconds(1, set_parameter, msg);

    syslog(LOG_INFO, "MulticastPort changed to '%s'", value);
}

int main(int argc, char** argv) {
    (void)argc;
    GError* error = NULL;
    char* app_name = basename(argv[0]);

    openlog(app_name, LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Starting %s", app_name);

    /* TODO 1: Add settingPage and paramConfig to manifest.json. */
    /* TODO 2: Add axparameter to the Makefile PKGS line. */

    /* TODO 3: Create the AXParameter handle here. */

    /* TODO 4: Register callbacks for MulticastAddress and MulticastPort here. */

    /* TODO 5: Create the GLib main loop, register signals, run, then clean up. */

    closelog();
    return EXIT_SUCCESS;
}
