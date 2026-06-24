# Overlay2 Draw Views Exercise

This exercise renders different overlay graphics depending on the VDO view metadata. It creates one overlay per overlay-capable stream, reads the stream `camera` value as `view_id`, draws a different Cairo shape for each view group, copies the ARGB pixels into an `axoverlay2` buffer, and submits the buffer.

The helper functions in `app/overlay2_draw_views.c` are already provided. Complete only `main()` one TODO at a time.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 gio-unix-2.0 glib-2.0 cairo vdostream axoverlay2
```

## Step 2: Add manifest resources

After `schemaVersion`, add the `resources` block below. Remember to add a comma after the `schemaVersion` line and keep the comma after the closing brace of `resources`.

```json
"resources": {
    "overlay": {
        "enabled": true,
        "required": true
    }
},
```

## Step 3: Start axoverlay2 and app state

Paste this where the file says `TODO 1`:

```c
GError* error = NULL;
axo_err* axo_error = NULL;
VdoMap* stream_filter = NULL;
GIOChannel* vdo_channel = NULL;
unsigned vdo_watch_id = 0;
bool axo_running = false;
int ret = 0;

openlog("overlay2_draw_views", LOG_PID, LOG_USER);

if (!axo_start(NULL, &axo_error)) {
    syslog(LOG_ERR, "Failed to start axoverlay2: %s", axo_err_get_message(axo_error));
    ret = 1;
    goto out;
}
axo_running = true;

overlay_table = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, overlay_record_deleter);
main_loop = g_main_loop_new(NULL, FALSE);
g_timeout_add(tick_period_us / 1000, animation_tick_callback, NULL);
```

The timer redraws every active overlay. Each overlay record stores `stream_id`, `view_id`, aligned dimensions, and a reusable Cairo surface.

## Step 4: Attach the VDO overlay filter

Paste this where the file says `TODO 2`:

```c
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

This makes VDO report streams where overlays can be created.

## Step 5: Watch VDO events and signals

Paste this where the file says `TODO 3`:

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
```

`stream_event_callback()` opens each stream, reads width and height, reads `camera` as the view id, and calls `create_overlay(stream_id, width, height, view_id)`.

## Step 6: Run the app

Paste this where the file says `TODO 4`:

```c
g_main_loop_run(main_loop);
```

While the loop runs, `render_frame()` uses `view_id % 3` to choose a yellow rectangle, blue circle, or green triangle for each overlay.

## Step 7: Clean up

Paste this where the file says `TODO 5`:

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
docker build --tag draw-views --build-arg ARCH=aarch64 .
docker cp $(docker create draw-views):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Test

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `overlay2/draw-views` in `axis-acap-tip-workshop`.
