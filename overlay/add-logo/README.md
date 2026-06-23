# Add Logo Exercise

This exercise draws a PNG logo over video using `axoverlay`, Cairo, and an ARGB32 overlay.

`app/add_logo.c` keeps the PNG loading helper, signal handler, and callbacks in place. Complete the TODOs in `main()` with the snippets below.

The important callback flow is:

- `adjustment_cb` keeps the overlay aligned with the current stream size and rotation.
- `render_overlay_cb` receives the Cairo context and calls `draw_logo()`.
- `draw_logo()` loads the PNG, scales it, and paints it with Cairo.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 glib-2.0 cairo
```

## Step 2: Add overlay access to manifest.json

Open `app/manifest.json`.

After `schemaVersion`, add:

```json
"resources": {
    "dbus": {
        "requiredMethods": [
            "com.axis.Graphics2.*",
            "com.axis.Overlay2.*"
        ]
    }
},
```

## Step 3: Create the main loop and check the backend

Open `app/add_logo.c`.

Paste this where the file says `TODO 1`:

```c
setenv("XDG_CACHE_HOME", "/usr/local/packages/add_logo/localdata", 1);

GMainLoop* loop = NULL;
GError* error = NULL;
gint camera_height = 0;
gint camera_width = 0;

openlog(NULL, LOG_PID, LOG_USER);

loop = g_main_loop_new(NULL, FALSE);
g_unix_signal_add(SIGTERM, signal_handler, loop);
g_unix_signal_add(SIGINT, signal_handler, loop);

if (!axoverlay_is_backend_supported(AXOVERLAY_CAIRO_IMAGE_BACKEND)) {
    syslog(LOG_ERR, "AXOVERLAY_CAIRO_IMAGE_BACKEND is not supported");
    return 1;
}
```

## Step 4: Register the overlay callbacks

Paste this where the file says `TODO 2`:

```c
struct axoverlay_settings settings;
axoverlay_init_axoverlay_settings(&settings);
settings.render_callback = render_overlay_cb;
settings.adjustment_callback = adjustment_cb;
settings.select_callback = NULL;
settings.backend = AXOVERLAY_CAIRO_IMAGE_BACKEND;

axoverlay_init(&settings, &error);
if (error != NULL) {
    syslog(LOG_ERR, "Failed to initialize axoverlay: %s", error->message);
    g_error_free(error);
    return 1;
}
```

## Step 5: Read the camera resolution

Paste this where the file says `TODO 3`:

```c
camera_width = axoverlay_get_max_resolution_width(1, &error);
if (error != NULL) {
    syslog(LOG_ERR, "Failed to get max resolution width: %s", error->message);
    g_error_free(error);
    axoverlay_cleanup();
    return 1;
}

camera_height = axoverlay_get_max_resolution_height(1, &error);
if (error != NULL) {
    syslog(LOG_ERR, "Failed to get max resolution height: %s", error->message);
    g_error_free(error);
    axoverlay_cleanup();
    return 1;
}

syslog(LOG_INFO, "Max resolution (width x height): %i x %i", camera_width, camera_height);
```

## Step 6: Create the ARGB32 logo overlay

Paste this where the file says `TODO 4`:

```c
struct axoverlay_overlay_data data;
setup_axoverlay_data(&data);
data.width = camera_width;
data.height = camera_height;
data.colorspace = AXOVERLAY_COLORSPACE_ARGB32;

overlay_id = axoverlay_create_overlay(&data, NULL, &error);
if (error != NULL) {
    syslog(LOG_ERR, "Failed to create overlay: %s", error->message);
    g_error_free(error);
    axoverlay_cleanup();
    return 1;
}
```

ARGB32 is used because the PNG logo has normal alpha transparency.

## Step 7: Redraw and run

Paste this where the file says `TODO 5`:

```c
axoverlay_redraw(&error);
if (error != NULL) {
    syslog(LOG_ERR, "Failed to draw overlay: %s", error->message);
    axoverlay_destroy_overlay(overlay_id, NULL);
    axoverlay_cleanup();
    g_error_free(error);
    return 1;
}

g_main_loop_run(loop);
```

## Step 8: Clean up

Paste this where the file says `TODO 6`:

```c
axoverlay_destroy_overlay(overlay_id, &error);
if (error != NULL) {
    syslog(LOG_ERR, "Failed to destroy overlay: %s", error->message);
    g_error_free(error);
    axoverlay_cleanup();
    g_main_loop_unref(loop);
    return 1;
}

axoverlay_cleanup();
g_main_loop_unref(loop);
closelog();
return 0;
```

## Build

From this example directory:

```sh
docker build --tag add-logo --build-arg ARCH=aarch64 .
docker cp $(docker create add-logo):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `overlay/add-logo` in `axis-acap-tip-workshop`.
