# Parameter Runtime Exercise

This exercise shows how an ACAP application can create and manage application parameters from C code instead of declaring them in `manifest.json`.

The signal handler and small helper wrappers are already present in `app/parameter_runtime.c`. Add the missing AXParameter workflow in the TODO sections.

## Step 1: Add AXParameter to the Makefile

Open `app/Makefile` and add `axparameter` to `PKGS`:

```make
PKGS = gio-2.0 gio-unix-2.0 axparameter
```

## Step 2: Add system parameter access to manifest.json

Open `app/manifest.json`.

After `schemaVersion`, add the `resources` block below. Remember to add a comma after the `schemaVersion` line and keep the comma after the closing brace of `resources`.

```json
"resources": {
    "linux": {
        "user": {
            "groups": [
                "admin"
            ]
        }
    }
},
```

This gives the application user access to the `admin` Linux group, which is required when the app needs to create, remove, and update parameters at runtime.

## Step 3: Add the parameter callback

Paste this callback at `TODO 2` in `app/parameter_runtime.c`:

```c
static void acap_parameter_changed(const gchar* name,
                                   const gchar* value,
                                   gpointer user_data) {
    (void)user_data;
    const char* local_name = &name[strlen("root." APP_NAME ".")];
    syslog(LOG_INFO, "%s was changed to '%s'", local_name, value);
}
```

## Step 4: Create the AXParameter handle

Paste this in `main()` at `TODO 3`:

```c
AXParameter* handle = ax_parameter_new(APP_NAME, &error);
if (!handle) {
    panic("ax_parameter_new failed: %s", error->message);
}
```

## Step 5: Add and change runtime parameters

Paste this at `TODO 4`:

```c
add_parameter(handle, "ParameterRuntime", "no", "string");
add_parameter(handle, "ParameterToRemoveRuntime", "yes", "string");

print_parameters(handle);
remove_parameter(handle, "ParameterToRemoveRuntime");
set_parameter(handle, "ParameterRuntime", "yes");
print_parameters(handle);
```

## Step 6: Register callbacks

Paste this at `TODO 5`:

```c
if (!ax_parameter_register_callback(handle,
                                    "ParameterRuntime",
                                    acap_parameter_changed,
                                    NULL,
                                    &error)) {
    panic("register callback failed: %s", error->message);
}
```

## Step 7: Run the GLib loop and clean up

Paste this at `TODO 6`:

```c
loop = g_main_loop_new(NULL, FALSE);
g_unix_signal_add(SIGTERM, signal_handler, loop);
g_unix_signal_add(SIGINT, signal_handler, loop);
g_main_loop_run(loop);

g_main_loop_unref(loop);
ax_parameter_free(handle);
```

## Build

From this example directory:

```sh
docker build --tag parameter-runtime --build-arg ARCH=aarch64 .
docker cp $(docker create parameter-runtime):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the application, start it, and follow the [test guide](.test/test.md). You should see the runtime parameters being added, listed, modified, and listened to for later changes.
