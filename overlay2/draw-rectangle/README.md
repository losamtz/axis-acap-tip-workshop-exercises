# Draw Rectangle Exercise

This exercise is based on `overlay2/draw-rectangle` from the complete `axis-acap-tip-workshop` repository.

`app/overlay2_draw_rectangle.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Review manifest configuration

This example uses manifest entries for `resources`. Review `app/manifest.json` before building and keep these entries aligned with the README workflow.

## Step 2: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 glib-2.0 cairo vdostream axoverlay2
```

## Step 3: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
GError* error = NULL;
    axo_err* axo_error = NULL;
    VdoMap* stream_filter = NULL;
    GIOChannel* vdo_channel = NULL;
    unsigned vdo_watch_id = 0;
    bool axo_running = false;
    int ret = 0;

    openlog("overlay2_draw_rectangle", LOG_PID, LOG_USER);

    if (!axo_start(NULL, &axo_error)) {
        syslog(LOG_ERR, "Failed to start axoverlay2: %s", axo_err_get_message(axo_error));
        ret = 1;
        goto out;
    }
    axo_running = true;

    overlay_table = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, overlay_record_deleter);
    main_loop = g_main_loop_new(NULL, FALSE);
    g_timeout_add(tick_period_us / 1000, animation_tick_callback, NULL);

    vdo_event_stream = vdo_stream_get(0, &error);
    if (!vdo_event_stream) {
        syslog(LOG_ERR, "Failed to open VDO stream 0: %s", error->message);
        ret = 1;
        goto out;
    }

    stream_filter = vdo_map_new();
    vdo_map_set_string(stream_filter, "filter", "overlay");

    if (!vdo_stream_attach(vdo_event_stream, stream_filter, &error)) {
        syslog(LOG_ERR, "Failed to attach VDO overlay filter: %s", error->message);
        ret = 1;
        goto out;
    }
```

## Step 4: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
int stream_event_fd = vdo_stream_get_event_fd(vdo_event_stream, &error);
    if (stream_event_fd < 0) {
        syslog(LOG_ERR, "Failed to get VDO event fd: %s", error->message);
        ret = 1;
        goto out;
    }

    vdo_channel = g_io_channel_unix_new(stream_event_fd);
    vdo_watch_id = g_io_add_watch(vdo_channel,
                                  G_IO_IN | G_IO_PRI | G_IO_ERR | G_IO_HUP,
                                  stream_event_callback,
                                  NULL);
    if (!vdo_watch_id) {
        syslog(LOG_ERR, "Failed to add VDO event fd to GLib loop");
        ret = 1;
        goto out;
    }

    g_unix_signal_add(SIGINT, signal_callback, NULL);
    g_unix_signal_add(SIGTERM, signal_callback, NULL);

    g_main_loop_run(main_loop);
```

## Step 5: Add main runtime flow snippet

Paste this into `main()` at the next TODO position:

```c
out:
    if (vdo_watch_id)
        g_source_remove(vdo_watch_id);
    if (vdo_channel)
        g_io_channel_unref(vdo_channel);
    if (axo_running)
        axo_stop(NULL);
    if (vdo_event_stream)
        g_object_unref(vdo_event_stream);
    if (stream_filter)
        g_object_unref(stream_filter);
    g_clear_error(&error);
    axo_err_clear(&axo_error);
    if (main_loop)
        g_main_loop_unref(main_loop);
    if (overlay_table)
        g_hash_table_unref(overlay_table);

    closelog();
    return ret;
```

## Build

From this example directory:

```sh
docker build --tag draw-rectangle --build-arg ARCH=aarch64 .
docker cp $(docker create draw-rectangle):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `overlay2/draw-rectangle` in `axis-acap-tip-workshop`.
