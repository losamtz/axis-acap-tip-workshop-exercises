# Draw Views Exercise

This exercise draws different Cairo shapes over multiple views using `axoverlay`.

`app/draw_views.c` keeps the drawing helpers and callbacks in place. Complete the TODOs in `main()` with the snippets below.

The important callback flow is:

- `adjustment_cb` sizes the overlay for each stream and handles rotation.
- `render_overlay_cb` checks the stream camera/view and draws rectangle, circle, or triangle shapes.
- `axoverlay_redraw()` asks axoverlay to render the current frame.

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

Open `app/draw_views.c`.

Paste this where the file says `TODO 1`:

```c
setenv("XDG_CACHE_HOME", "/usr/local/packages/draw_views/localdata", 1);

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

## Step 5: Configure shape colors

Paste this where the file says `TODO 3`:

```c
if (!setup_palette_color(0, 0, 0, 0, 0) ||
    !setup_palette_color(1, 255, 0, 0, 255) ||
    !setup_palette_color(2, 0, 255, 0, 255) ||
    !setup_palette_color(3, 0, 0, 255, 255) ||
    !setup_palette_color(4, 255, 255, 0, 255)) {
    syslog(LOG_ERR, "Failed to setup palette colors");
    axoverlay_cleanup();
    return 1;
}
```

The helper functions draw with palette indexes, and `index2cairo()` converts them to Cairo color values.

## Step 6: Read the camera resolution

Paste this where the file says `TODO 4`:

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

## Step 7: Create the overlay

Paste this where the file says `TODO 5`:

```c
struct axoverlay_overlay_data data;
setup_axoverlay_data(&data);
data.width = camera_width;
data.height = camera_height;
data.colorspace = AXOVERLAY_COLORSPACE_4BIT_PALETTE;

overlay_id = axoverlay_create_overlay(&data, NULL, &error);
if (error != NULL) {
    syslog(LOG_ERR, "Failed to create overlay: %s", error->message);
    g_error_free(error);
    axoverlay_cleanup();
    return 1;
}
```

## Step 8: Redraw and run

Paste this where the file says `TODO 6`:

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

## Step 9: Clean up

Paste this where the file says `TODO 7`:

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
docker build --tag draw-views --build-arg ARCH=aarch64 .
docker cp $(docker create draw-views):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `overlay/draw-views` in `axis-acap-tip-workshop`.
