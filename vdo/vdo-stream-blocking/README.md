# Vdo Stream Blocking Exercise

This exercise creates the simplest VDO frame loop: an H.264 stream where `vdo_stream_get_buffer()` blocks until a frame is available.

`app/vdo_stream_blocking.c` keeps the error handling helper in place. Complete the TODOs in `main()` with the snippets below.

The important ownership flow is:

```text
create stream -> start stream -> get buffer -> inspect frame -> return buffer
```

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 gio-unix-2.0 vdostream
```

## Step 2: Add video access to manifest.json

Open `app/manifest.json`.

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

This gives the app access to the camera video pipeline.

## Step 3: Initialize logging and variables

Open `app/vdo_stream_blocking.c`.

Paste this where the file says `TODO 1`:

```c
GError* error = NULL;
VdoMap* settings = NULL;
VdoStream* stream = NULL;
VdoMap* info = NULL;

openlog(NULL, LOG_PID, LOG_USER);
syslog(LOG_INFO, "Starting %s", argv[0]);
```

## Step 4: Create the H.264 blocking stream

Paste this where the file says `TODO 2`:

```c
settings = vdo_map_new();
vdo_map_set_uint32(settings, "channel", 1u);
vdo_map_set_uint32(settings, "format", VDO_FORMAT_H264);

stream = vdo_stream_new(settings, NULL, &error);
if (!stream) {
    return handle_vdo_failed(error);
}
```

The stream is blocking because the code does not set `socket.blocking` to `FALSE`.

## Step 5: Start the stream and log stream info

Paste this where the file says `TODO 3`:

```c
if (!vdo_stream_start(stream, &error)) {
    return handle_vdo_failed(error);
}

info = vdo_stream_get_info(stream, &error);
if (!info) {
    panic("%s: Failed to get vdo stream info: %s", __func__, error->message);
}

syslog(LOG_INFO,
       "Starting stream resolution: %ux%u, at %u fps",
       vdo_map_get_uint32(info, "width", 0),
       vdo_map_get_uint32(info, "height", 0),
       (unsigned int)(vdo_map_get_double(info, "framerate", 0.0) + 0.5));
```

Always read back stream info. VDO may adjust requested settings based on product capabilities.

## Step 6: Fetch 10 buffers

Paste this where the file says `TODO 4`:

```c
for (size_t i = 0; i < 10;) {
    VdoBuffer* vdo_buf = vdo_stream_get_buffer(stream, &error);
    if (!vdo_buf && g_error_matches(error, VDO_ERROR, VDO_ERROR_NO_DATA)) {
        g_clear_error(&error);
        continue;
    }
    if (!vdo_buf) {
        return handle_vdo_failed(error);
    }
```

For a blocking stream, `vdo_stream_get_buffer()` waits until a frame is available.

## Step 7: Inspect and return each buffer

Paste this where the file says `TODO 5`:

```c
    VdoFrame* frame = vdo_buffer_get_frame(vdo_buf);
    gint64 pts = vdo_frame_get_timestamp(frame);

    syslog(LOG_INFO,
           "Format: H264 - Timestamp: %u us - Frame: %u, Size: %zu",
           (unsigned int)pts,
           vdo_frame_get_sequence_nbr(frame),
           vdo_frame_get_size(frame));

    if (!vdo_stream_buffer_unref(stream, &vdo_buf, &error)) {
        return handle_vdo_failed(error);
    }

    i += 1;
}
```

Returning the buffer is mandatory. VDO owns the buffer pool and may reuse the memory immediately.

## Step 8: Clean up

Paste this where the file says `TODO 6`:

```c
if (info) {
    g_object_unref(info);
}
if (stream) {
    g_object_unref(stream);
}
if (settings) {
    g_object_unref(settings);
}

closelog();
return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag vdo-stream-blocking --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-stream-blocking):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `vdo/vdo-stream-blocking` in `axis-acap-tip-workshop`.
