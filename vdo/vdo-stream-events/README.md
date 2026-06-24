# VDO Stream Events Exercise

This exercise listens for VDO stream events and logs metadata for overlay-capable streams. The helper functions and callback are already in `app/vdo_stream_events.c`; complete `main()` one TODO at a time.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 gio-unix-2.0 glib-2.0 vdostream
```

## Step 2: Add manifest resources

After `schemaVersion`, add the `resources` block below. Remember to add a comma after the `schemaVersion` line and keep the comma after the closing brace of `resources`.

```json
"resources": {
    "linux": {
        "user": {
            "groups": ["video"]
        }
    }
},
```

## Step 3: Initialize the app

Paste this where the file says `TODO 1`:

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
```

This prepares shared state for the stream-event callback and makes `SIGINT`/`SIGTERM` stop the GLib main loop cleanly.

## Step 4: Attach the stream-event filter

Paste this where the file says `TODO 2`:

```c
event_stream = vdo_stream_get(0, &error);
if (!event_stream)
    panic("Failed to open VDO stream 0: %s", error->message);

stream_filter = vdo_map_new();
vdo_map_set_string(stream_filter, "filter", "overlay");

if (!vdo_stream_attach(event_stream, stream_filter, &error))
    panic("Failed to attach overlay stream filter: %s", error->message);
```

The stream filter asks VDO to report streams that can be used for overlay-related work. The callback later receives `EXISTING`, `CREATED`, and `CLOSED` events.

## Step 5: Watch the event fd

Paste this where the file says `TODO 3`:

```c
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
```

`stream_event_callback()` calls `vdo_stream_get_event()`, logs the event type and stream id, and calls `log_stream_info()` for existing and newly created streams.

## Step 6: Run the main loop

Paste this where the file says `TODO 4`:

```c
syslog(LOG_INFO, "Waiting for overlay-capable stream events");
g_main_loop_run(main_loop);
```

The app stays here until a signal handler or event error asks the main loop to quit.

## Step 7: Clean up

Paste this where the file says `TODO 5`:

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

## Test

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `vdo/vdo-stream-events` in `axis-acap-tip-workshop`.
