# Parameter Manifest Exercise

This exercise is based on the corresponding complete example in `axis-acap-tip-workshop`.
The source file `app/parameter_manifest.c` has been reduced to a small TODO skeleton.

Your task is to rebuild the application flow by pasting the snippet below into `app/parameter_manifest.c`.
The snippet is intentionally kept in the README so you can read the sequence before editing the C file.

## What to do

1. Open `app/parameter_manifest.c`.
2. Replace the skeleton implementation with the code from **Implementation snippet** below.
3. Read through the code and identify the API setup, runtime loop, and cleanup flow.
4. Build the package with the commands in **Build**.


## Implementation snippet

Paste this into `app/parameter_manifest.c`:

```c
#include <axsdk/axparameter.h>
#include <glib-unix.h>
#include <stdbool.h>
#include <syslog.h>

#define APP_NAME "parameter_manifest"



static gboolean signal_handler(gpointer loop) {
    g_main_loop_quit((GMainLoop*)loop);
    syslog(LOG_INFO, "Application was stopped by SIGTERM or SIGINT.");
    return G_SOURCE_REMOVE;
}
// Print an error to syslog and exit the application if a fatal error occurs.
__attribute__((noreturn)) __attribute__((format(printf, 1, 2))) static void
panic(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    vsyslog(LOG_ERR, format, arg);
    va_end(arg);
    exit(1);
}


static void acap_parameter_changed(const gchar* name, const gchar* value, gpointer handle_void_ptr){

    // This function is called when the parameter changes.
    (void)handle_void_ptr; // Unused parameter, but required by the callback signature.
    
    const char* name_without_qualifiers = &name[strlen("root." APP_NAME ".")];
    syslog(LOG_INFO, "%s was changed to '%s' just now", name_without_qualifiers, value);

    //free(name_without_qualifiers);
    //free(value);
}
int main(void) {
    GError* error   = NULL;
    GMainLoop* loop = NULL;

    openlog(APP_NAME, LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Starting %s", APP_NAME);

    // 1. Create a new AXParameter handle.
    AXParameter* handle = ax_parameter_new(APP_NAME, &error);
    if(handle == NULL)
        panic("%s", error->message);

    syslog(LOG_INFO, "Starting handle");

    // 2. Act on changes to IsCustomized as soon as they happen.
    if(!ax_parameter_register_callback(handle, "ParameterManifest", acap_parameter_changed, NULL, &error))
        panic("%s", error->message);
    
    // 3. Start listening to callbacks by launching a GLib main loop.
    loop = g_main_loop_new(NULL, FALSE);

    g_unix_signal_add(SIGTERM, signal_handler, loop);
    g_unix_signal_add(SIGINT, signal_handler, loop);
    g_main_loop_run(loop);

    
    g_main_loop_unref(loop);
    ax_parameter_free(handle);
}
```

## Build

From this example directory:

```sh
docker build --tag parameter-manifest --build-arg ARCH=aarch64 .
docker cp $(docker create parameter-manifest):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera from the Apps page or with your usual ACAP install flow.
If the application exposes HTTP endpoints or overlays, use the behavior described by the code comments and the parent module README to verify it.

## Reference

The complete version lives in the original `axis-acap-tip-workshop` repository under the same relative path:

`parameter/parameter-manifest`
