# Vdo Stream Events Exercise

This exercise is based on `vdo/vdo-stream-events` from the complete `axis-acap-tip-workshop` repository.

`app/vdo_stream_events.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Review manifest configuration

This example uses manifest entries for `resources`. Review `app/manifest.json` before building and keep these entries aligned with the README workflow.

## Step 2: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 gio-unix-2.0 glib-2.0 vdostream
```

## Step 3: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
GError* error = NULL;
    VdoMap* stream_filter = NULL;
    GIOChannel* channel = NULL;
    guint watch_id = 0;

    openlog("vdo_stream_events", LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Starting VDO stream events example");

    main_loop = g_main_loop_new(NULL, FALSE);
    g_unix_signal_add(SIGINT, signal_handler, NULL);
    g_unix_signal_add(SIGTERM, signal_handler, NULL);

    event_stream = vdo_stream_get(0, &error);
    if (!event_stream)
        panic("Failed to open VDO stream 0: %s", error->message);

    stream_filter = vdo_map_new();
    vdo_map_set_string(stream_filter, "filter", "overlay");

    if (!vdo_stream_attach(event_stream, stream_filter, &error))
        panic("Failed to attach overlay stream filter: %s", error->message);

    int event_fd = vdo_stream_get_event_fd(event_stream, &error);
    if (event_fd < 0)
        panic("Failed to get VDO event fd: %s", error->message);

    channel = g_io_channel_unix_new(event_fd);
    watch_id = g_io_add_watch(channel,
                              G_IO_IN | G_IO_PRI | G_IO_ERR | G_IO_HUP,
                              stream_event_callback,
                              NULL);
    if (!watch_id)
        panic("Failed to add VDO event fd to GLib main loop");

    syslog(LOG_INFO, "Waiting for overlay-capable stream events");
    g_main_loop_run(main_loop);
```

## Step 4: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
if (watch_id)
        g_source_remove(watch_id);
    if (channel)
        g_io_channel_unref(channel);
    if (stream_filter)
        g_object_unref(stream_filter);
    if (event_stream)
        g_object_unref(event_stream);
    if (main_loop)
        g_main_loop_unref(main_loop);
    g_clear_error(&error);

    syslog(LOG_INFO, "Stopped VDO stream events example");
    closelog();
    return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag vdo-stream-events --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-stream-events):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `vdo/vdo-stream-events` in `axis-acap-tip-workshop`.
