# Vdo Stream RGB Exercise

This exercise requests raw RGB frames from VDO using the convenience constructor `vdo_stream_rgb_new()`.

It keeps the non-blocking `poll()` pattern from `vdo-stream-non-block`, but changes the stream format from encoded H.264 to raw RGB pixels.

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

Open `app/vdo_stream_rgb.c`.

Paste this where the file says `TODO 1`:

```c
GError* error = NULL;
VdoStream* stream = NULL;
VdoMap* info = NULL;

openlog(NULL, LOG_PID, LOG_USER);
syslog(LOG_INFO, "Starting %s", argv[0]);
```

## Step 4: Create the RGB stream

Paste this where the file says `TODO 2`:

```c
stream = vdo_stream_rgb_new(NULL,
                            1u,
                            (VdoResolution){.width = 640u, .height = 360u},
                            &error);
if (!stream) {
    return handle_vdo_failed(error);
}
```

This convenience API requests RGB frames from channel 1 at 640 x 360.

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
       "Starting stream format RGB - resolution: %ux%u, pitch: %u, at %u fps",
       vdo_map_get_uint32(info, "width", 0),
       vdo_map_get_uint32(info, "height", 0),
       vdo_map_get_uint32(info, "pitch", 0),
       (unsigned int)(vdo_map_get_double(info, "framerate", 0.0) + 0.5));
```

Reading stream info shows what VDO actually created, including RGB pitch.

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

The polling logic is unchanged from the non-blocking H.264 example.

## Step 7: Inspect and return each RGB buffer

Paste this where the file says `TODO 5`:

```c
    VdoFrame* frame = vdo_buffer_get_frame(vdo_buf);
    gint64 pts = vdo_frame_get_timestamp(frame);

    syslog(LOG_INFO,
           "Format: RGB - Timestamp: %u us - Frame: %u, Size: %zu",
           (unsigned int)pts,
           vdo_frame_get_sequence_nbr(frame),
           vdo_frame_get_size(frame));

    if (!vdo_stream_buffer_unref(stream, &vdo_buf, &error)) {
        return handle_vdo_failed(error);
    }

    i += 1;
}
```

RGB buffers are raw pixel data, but the ownership rule is still the same: return each buffer to VDO after use.

## Step 8: Clean up

Paste this where the file says `TODO 6`:

```c
if (info) {
    g_object_unref(info);
}
if (stream) {
    g_object_unref(stream);
}

closelog();
return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag vdo-stream-rgb --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-stream-rgb):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `vdo/vdo-stream-rgb` in `axis-acap-tip-workshop`.
