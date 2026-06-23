# Parameter Manifest Exercise

This exercise teaches how an application parameter declared in `manifest.json` becomes available to C code through the AXParameter API.

The app starts with three missing pieces:

- `app/manifest.json` does not yet declare the `ParameterManifest` parameter.
- `app/Makefile` links GLib, but not `axparameter` yet.
- `app/parameter_manifest.c` contains the basic includes and signal handler, but the AXParameter flow is incomplete.

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

## Step 3: Add the parameter callback

Open `app/parameter_manifest.c`.

Paste this where the file says `TODO 3`:

```c
static void acap_parameter_changed(const gchar* name, const gchar* value, gpointer user_data) {
    (void)user_data;

    const char* prefix = "root." APP_NAME ".";
    const char* short_name = g_str_has_prefix(name, prefix) ? name + strlen(prefix) : name;

    syslog(LOG_INFO, "%s was changed to '%s'", short_name, value);
}
```

This callback runs when the `ParameterManifest` parameter changes from the app settings page or VAPIX.

## Step 4: Create the AXParameter handle

In `main()`, paste this where the file says `TODO 4`:

```c
AXParameter* handle = ax_parameter_new(APP_NAME, &error);
if (!handle) {
    syslog(LOG_ERR, "ax_parameter_new failed");
    return EXIT_FAILURE;
}
```

`ax_parameter_new()` opens the application's parameter namespace. The `APP_NAME` value must match `acapPackageConf.setup.appName` in `manifest.json`.

## Step 5: Register the callback

Paste this where the file says `TODO 5`:

```c
ax_parameter_register_callback(handle, "ParameterManifest", acap_parameter_changed, NULL, &error);
```

This connects changes to `ParameterManifest` with the callback function you added in Step 3.

## Step 6: Run the main loop and clean up

Paste this where the file says `TODO 6`:

```c
loop = g_main_loop_new(NULL, FALSE);
g_unix_signal_add(SIGTERM, signal_handler, loop);
g_unix_signal_add(SIGINT, signal_handler, loop);
g_main_loop_run(loop);

g_main_loop_unref(loop);
ax_parameter_free(handle);
```

The GLib main loop keeps the application alive so the callback can run. The signal handler is already included in the C file.

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
