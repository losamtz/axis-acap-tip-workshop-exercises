# Draw Text Exercise

This exercise draws dynamic text over video using `axoverlay`, Cairo, and an ARGB32 overlay.

`app/draw_text.c` keeps the text drawing helper, redraw timer, signal handler, and callbacks in place. Complete the TODOs in `main()` with the snippets below.

The important callback flow is:

- `adjustment_cb` sizes the overlay for the current stream and rotation.
- `render_overlay_cb` clears the overlay and calls `draw_text()`.
- `update_overlay_cb` changes the text position and calls `axoverlay_redraw()`.

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

Open `app/draw_text.c`.

Paste this where the file says `TODO 1`:

```c
setenv("XDG_CACHE_HOME", "/usr/local/packages/draw_text/localdata", 1);

GMainLoop* loop = NULL;
GError* error = NULL;
GError* error_text = NULL;
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

This registers the render callback that performs the Cairo text drawing.

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

## Step 6: Create the ARGB32 text overlay

Paste this where the file says `TODO 4`:

```c
struct axoverlay_overlay_data data_text;
setup_axoverlay_data(&data_text);
data_text.width = camera_width;
data_text.height = camera_height;
data_text.colorspace = AXOVERLAY_COLORSPACE_ARGB32;

overlay_id_text = axoverlay_create_overlay(&data_text, NULL, &error_text);
if (error_text != NULL) {
    syslog(LOG_ERR, "Failed to create text overlay: %s", error_text->message);
    g_error_free(error_text);
    axoverlay_cleanup();
    return 1;
}
```

ARGB32 lets Cairo draw anti-aliased text with normal RGBA colors.

## Step 7: Redraw, start the timer, and run

Paste this where the file says `TODO 5`:

```c
axoverlay_redraw(&error);
if (error != NULL) {
    syslog(LOG_ERR, "Failed to draw overlay: %s", error->message);
    axoverlay_destroy_overlay(overlay_id_text, NULL);
    axoverlay_cleanup();
    g_error_free(error);
    return 1;
}

animation_timer = g_timeout_add_seconds(1, update_overlay_cb, NULL);
g_main_loop_run(loop);
```

The timer updates the text state and requests a redraw every second.

## Step 8: Clean up

Paste this where the file says `TODO 6`:

```c
axoverlay_destroy_overlay(overlay_id_text, &error_text);
if (error_text != NULL) {
    syslog(LOG_ERR, "Failed to destroy text overlay: %s", error_text->message);
    g_error_free(error_text);
    axoverlay_cleanup();
    g_main_loop_unref(loop);
    return 1;
}

if (animation_timer > 0) {
    g_source_remove(animation_timer);
}

axoverlay_cleanup();
g_main_loop_unref(loop);
closelog();
return 0;
```

## Build

From this example directory:

```sh
docker build --tag draw-text --build-arg ARCH=aarch64 .
docker cp $(docker create draw-text):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `overlay/draw-text` in `axis-acap-tip-workshop`.
