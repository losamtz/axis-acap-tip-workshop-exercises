/*
 * Parameter runtime exercise skeleton.
 *
 * The signal handler and helper wrappers are provided. Follow README.md to add
 * the Makefile dependency, callback, AXParameter handle setup, runtime
 * parameter operations, callback registration, main loop, and cleanup.
 */

#include <axsdk/axparameter.h>
#include <glib-unix.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#define APP_NAME "parameter_runtime"

static gboolean signal_handler(gpointer loop) {
    g_main_loop_quit((GMainLoop*)loop);
    syslog(LOG_INFO, "Application was stopped by SIGTERM or SIGINT.");
    return G_SOURCE_REMOVE;
}

__attribute__((noreturn)) __attribute__((format(printf, 1, 2))) static void
panic(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    vsyslog(LOG_ERR, format, arg);
    va_end(arg);
    exit(EXIT_FAILURE);
}

static void add_parameter(AXParameter* handle,
                          const char* name,
                          const char* default_value,
                          const char* meta) {
    GError* error = NULL;
    if (!ax_parameter_add(handle, name, default_value, meta, &error)) {
        if (error->code == AX_PARAMETER_PARAM_ADDED_ERROR) {
            g_error_free(error);
            return;
        }
        panic("Failed to add parameter %s: %s", name, error->message);
    }
    syslog(LOG_INFO, "[add-param] Parameter '%s' was added", name);
}

static void set_parameter(AXParameter* handle, const char* name, const char* value) {
    GError* error = NULL;
    if (!ax_parameter_set(handle, name, value, TRUE, &error)) {
        panic("Failed to set parameter '%s': %s", name, error->message);
    }
    syslog(LOG_INFO, "[set-param] Parameter '%s' set to '%s'", name, value);
}

static void remove_parameter(AXParameter* handle, const char* name) {
    GError* error = NULL;
    if (!ax_parameter_remove(handle, name, &error)) {
        panic("Failed to remove parameter '%s': %s", name, error->message);
    }
    syslog(LOG_INFO, "[remove-param] Parameter '%s' removed", name);
}

static void print_parameters(AXParameter* handle) {
    GError* error = NULL;
    GList* list = ax_parameter_list(handle, &error);
    if (!list) {
        panic("Failed to list parameters: %s", error->message);
    }

    for (GList* item = list; item != NULL; item = g_list_next(item)) {
        syslog(LOG_INFO, "[list-param] %s", (gchar*)item->data);
        g_free(item->data);
    }
    g_list_free(list);
}

/* TODO 2: Paste the parameter callback from README.md here. */

int main(void) {
    GError* error = NULL;
    GMainLoop* loop = NULL;

    openlog(APP_NAME, LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Starting %s", APP_NAME);

    /* TODO 1: Add axparameter to the Makefile PKGS line. */

    /* TODO 3: Create the AXParameter handle here. */

    /* TODO 4: Add, list, remove, and set runtime parameters here. */

    /* TODO 5: Register callbacks for the runtime parameters here. */

    /* TODO 6: Create the GLib main loop, register signals, run, then clean up. */

    closelog();
    return EXIT_SUCCESS;
}
