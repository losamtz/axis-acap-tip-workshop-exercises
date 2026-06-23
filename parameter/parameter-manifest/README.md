# Parameter Manifest Exercise

This exercise teaches how an application parameter declared in `manifest.json` becomes available to C code through the AXParameter API.

The app starts with three missing pieces:

- `app/manifest.json` does not yet declare the `ParameterManifest` parameter.
- `app/Makefile` links GLib, but not `axparameter` yet.
- `app/parameter_manifest.c` is a small TODO skeleton.

Complete the exercise by following the six steps below.

## Step 1: Add the parameter to manifest.json

Open `app/manifest.json`.

Inside `acapPackageConf`, after the `setup` object, add the `configuration` block below. Remember to add a comma after the closing brace of `setup`.

```json
,
"configuration": {
    "paramConfig": [
        {
            "name": "ParameterManifest",
            "default": "no",
            "type": "string"
        }
    ]
}
```

The final structure should be:

```json
"acapPackageConf": {
    "setup": {
        "...": "..."
    },
    "configuration": {
        "paramConfig": [
            {
                "name": "ParameterManifest",
                "default": "no",
                "type": "string"
            }
        ]
    }
}
```

## Step 2: Add AXParameter to the Makefile

Open `app/Makefile`.

Find this line:

```make
PKGS = gio-2.0 gio-unix-2.0
```

Add `axparameter` to the end:

```make
PKGS = gio-2.0 gio-unix-2.0 axparameter
```

`gio-2.0` and `gio-unix-2.0` are already needed for the GLib main loop and Unix signal handling. `axparameter` adds the headers and linker flags for the AXParameter library.

## Step 3: Add headers and constants

Open `app/parameter_manifest.c`.

Replace the current includes and `APP_NAME` setup with:

```c
#include <axsdk/axparameter.h>
#include <glib-unix.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#define APP_NAME "parameter_manifest"
```

## Step 4: Add the signal handler

Paste this below the `APP_NAME` definition:

```c
static gboolean signal_handler(gpointer loop) {
    g_main_loop_quit((GMainLoop*)loop);
    syslog(LOG_INFO, "Application was stopped by SIGTERM or SIGINT.");
    return G_SOURCE_REMOVE;
}
```

## Step 5: Add the parameter callback

Paste this below `signal_handler()`:

```c
static void acap_parameter_changed(const gchar* name, const gchar* value, gpointer user_data) {
    (void)user_data;

    const char* prefix = "root." APP_NAME ".";
    const char* short_name = g_str_has_prefix(name, prefix) ? name + strlen(prefix) : name;

    syslog(LOG_INFO, "%s was changed to '%s'", short_name, value);
}
```

This callback runs when the `ParameterManifest` parameter changes from the app settings page or VAPIX.

## Step 6: Replace main with the AXParameter flow

Replace the current `main()` function with:

```c
int main(void) {
    GError* error = NULL;
    GMainLoop* loop = NULL;

    openlog(APP_NAME, LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Starting %s", APP_NAME);

    AXParameter* handle = ax_parameter_new(APP_NAME, &error);
    if (!handle) {
        syslog(LOG_ERR, "ax_parameter_new failed");
        return EXIT_FAILURE;
    }

    ax_parameter_register_callback(handle, "ParameterManifest", acap_parameter_changed, NULL, &error);

    loop = g_main_loop_new(NULL, FALSE);
    g_unix_signal_add(SIGTERM, signal_handler, loop);
    g_unix_signal_add(SIGINT, signal_handler, loop);
    g_main_loop_run(loop);

    g_main_loop_unref(loop);
    ax_parameter_free(handle);
    closelog();
    return EXIT_SUCCESS;
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

1. Install the `.eap` on the camera.
2. Start the app.
3. Open the app settings page and change `ParameterManifest`.
4. Check syslog. You should see a message showing the new parameter value.

You can compare your finished files with the complete example in:

`axis-acap-tip-workshop/parameter/parameter-manifest`
