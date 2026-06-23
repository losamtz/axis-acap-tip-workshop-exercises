# Draw Text Exercise

This exercise is based on `overlay/draw-text` from the complete `axis-acap-tip-workshop` repository.

`app/draw_text.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Review manifest configuration

This example uses manifest entries for `resources`. Review `app/manifest.json` before building and keep these entries aligned with the README workflow.

## Step 2: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 glib-2.0 cairo
```

## Step 3: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
// Set XDG cache home to application's localdata directory for fontconfig
    setenv("XDG_CACHE_HOME", "/usr/local/packages/axoverlay/localdata", 1);
    GMainLoop* loop    = NULL;
    GError* error      = NULL;
    GError* error_text = NULL;
    gint camera_height = 0;
    gint camera_width  = 0;

    openlog(NULL, LOG_PID, LOG_USER);

    //  Create a glib main loop
    loop = g_main_loop_new(NULL, FALSE);
    g_unix_signal_add(SIGTERM, signal_handler, loop);
    g_unix_signal_add(SIGINT, signal_handler, loop);

    if (!axoverlay_is_backend_supported(AXOVERLAY_CAIRO_IMAGE_BACKEND)) {
        syslog(LOG_ERR, "AXOVERLAY_CAIRO_IMAGE_BACKEND is not supported");
        return 1;
    }
```

## Step 4: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
//  Initialize the library
    struct axoverlay_settings settings;
    axoverlay_init_axoverlay_settings(&settings);
    settings.render_callback     = render_overlay_cb;
    settings.adjustment_callback = adjustment_cb;
    settings.select_callback     = NULL;
    settings.backend             = AXOVERLAY_CAIRO_IMAGE_BACKEND;
    axoverlay_init(&settings, &error);
    if (error != NULL) {
        syslog(LOG_ERR, "Failed to initialize axoverlay: %s", error->message);
        g_error_free(error);
        return 1;
    }
```

## Step 5: Add main runtime flow snippet

Paste this into `main()` at the next TODO position:

```c
// Get max resolution for width and height
    camera_width = axoverlay_get_max_resolution_width(1, &error);
    if (error != NULL) {
        syslog(LOG_ERR, "Failed to get max resolution width: %s", error->message);
        g_error_free(error);
        error = NULL;
    }

    camera_height = axoverlay_get_max_resolution_height(1, &error);
    if (error != NULL) {
        syslog(LOG_ERR, "Failed to get max resolution height: %s", error->message);
        g_error_free(error);
        error = NULL;
    }

    syslog(LOG_INFO, "Max resolution (width x height): %i x %i", camera_width, camera_height);
```

## Step 6: Add main processing loop snippet

Paste this into `main()` at the next TODO position:

```c
// Create an text overlay using ARGB32 color space
    struct axoverlay_overlay_data data_text;
    setup_axoverlay_data(&data_text);
    data_text.width      = camera_width;
    data_text.height     = camera_height;
    data_text.colorspace = AXOVERLAY_COLORSPACE_ARGB32;
    overlay_id_text      = axoverlay_create_overlay(&data_text, NULL, &error_text);
    if (error_text != NULL) {
        syslog(LOG_ERR, "Failed to create second overlay: %s", error_text->message);
        g_error_free(error_text);
        return 1;
    }
```

## Step 7: Add main cleanup snippet

Paste this into `main()` at the next TODO position:

```c
// Draw overlays
    axoverlay_redraw(&error);
    if (error != NULL) {
        syslog(LOG_ERR, "Failed to draw overlays: %s", error->message);
        axoverlay_destroy_overlay(overlay_id_text, &error_text);
        axoverlay_cleanup();
        g_error_free(error);
        g_error_free(error_text);
        return 1;
    }

    // Start animation timer
    animation_timer = g_timeout_add_seconds(1, update_overlay_cb, NULL);
```

## Step 8: Add main workflow part 6 snippet

Paste this into `main()` at the next TODO position:

```c
// Enter main loop
    g_main_loop_run(loop);

    axoverlay_destroy_overlay(overlay_id_text, &error_text);
    if (error_text != NULL) {
        syslog(LOG_ERR, "Failed to destroy second overlay: %s", error_text->message);
        g_error_free(error_text);
        return 1;
    }

    // Release library resources
    axoverlay_cleanup();

    // Release the animation timer
    g_source_remove(animation_timer);
```

## Step 9: Add main workflow part 7 snippet

Paste this into `main()` at the next TODO position:

```c
// Release main loop
    g_main_loop_unref(loop);

    return 0;
```

## Build

From this example directory:

```sh
docker build --tag draw-text --build-arg ARCH=aarch64 .
docker cp $(docker create draw-text):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `overlay/draw-text` in `axis-acap-tip-workshop`.
