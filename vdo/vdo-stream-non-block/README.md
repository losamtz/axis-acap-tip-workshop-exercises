# Vdo Stream Non Block Exercise

This exercise changes the previous blocking frame loop into a poll-driven loop. The app waits for the VDO stream fd to become readable, then calls `vdo_stream_get_buffer()`.

`app/vdo_stream_non_block.c` keeps the error handling helper in place. Complete the TODOs in `main()` with the snippets below.

The important non-blocking flow is:

```text
create non-blocking stream -> get stream fd -> poll -> get buffer -> inspect frame -> return buffer
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

## Step 3: Initialize logging and variables

Open `app/vdo_stream_non_block.c`.

Paste this where the file says `TODO 1`:

```c
GError* error = NULL;
VdoMap* settings = NULL;
VdoStream* stream = NULL;
VdoMap* info = NULL;

openlog(NULL, LOG_PID, LOG_USER);
syslog(LOG_INFO, "Starting %s", argv[0]);
```

## Step 4: Create a non-blocking H.264 stream

Paste this where the file says `TODO 2`:

```c
settings = vdo_map_new();
vdo_map_set_uint32(settings, "channel", 1u);
vdo_map_set_uint32(settings, "format", VDO_FORMAT_H264);
vdo_map_set_boolean(settings, "socket.blocking", FALSE);

stream = vdo_stream_new(settings, NULL, &error);
if (!stream) {
    return handle_vdo_failed(error);
}
```

With `socket.blocking` set to `FALSE`, frame availability is signaled through the stream fd.

## Step 5: Get the fd, start the stream, and log info

Paste this where the file says `TODO 3`:

```c
int fd = vdo_stream_get_fd(stream, &error);
if (fd < 0) {
    return handle_vdo_failed(error);
}

struct pollfd fds = {
    .fd = fd,
    .events = POLL_IN,
};

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

The fd becomes readable when VDO has data available.

## Step 6: Poll before fetching buffers

Paste this where the file says `TODO 4`:

```c
for (size_t i = 0; i < 10;) {
    int status = 0;
    do {
        status = poll(&fds, 1, -1);
    } while (status == -1 && errno == EINTR);

    if (status < 0) {
        panic("Failed to poll with status %d", status);
    }

    VdoBuffer* vdo_buf = vdo_stream_get_buffer(stream, &error);
    if (!vdo_buf && g_error_matches(error, VDO_ERROR, VDO_ERROR_NO_DATA)) {
        g_clear_error(&error);
        continue;
    }
    if (!vdo_buf) {
        return handle_vdo_failed(error);
    }
```

Even after `poll()`, handle `VDO_ERROR_NO_DATA`; transient stream changes can still race with the read.

## Step 7: Inspect and return each buffer

Paste this where the file says `TODO 5`:

```c
    VdoFrame* frame = vdo_buffer_get_frame(vdo_buf);
    gint64 pts = vdo_frame_get_timestamp(frame);

    syslog(LOG_INFO,
           "Timestamp: %u us - Frame: %u, Size: %zu",
           (unsigned int)pts,
           vdo_frame_get_sequence_nbr(frame),
           vdo_frame_get_size(frame));

    if (!vdo_stream_buffer_unref(stream, &vdo_buf, &error)) {
        return handle_vdo_failed(error);
    }

    i += 1;
}
```

The buffer ownership rule is unchanged from the blocking example.

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
docker build --tag vdo-stream-non-block --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-stream-non-block):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `vdo/vdo-stream-non-block` in `axis-acap-tip-workshop`.
