# Vdo DMA-BUFs Exercise

This exercise shows how VDO can expose frame memory through file descriptors. It does not use larod. It only inspects VDO buffers, maps the fd with `mmap()`, logs a small byte sample, and returns the buffer to VDO.

`app/vdo_dma_bufs.c` keeps the stream creation helper, stream-info logger, and DMA-BUF inspection helper in place. Complete the TODOs in `main()` with the snippets below.

The important DMA-BUF flow is:

```text
poll stream fd -> get VdoBuffer -> read fd/offset/capacity -> mmap -> inspect bytes -> munmap -> return VdoBuffer
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

## Step 3: Initialize logging, signals, and settings

Open `app/vdo_dma_bufs.c`.

Paste this where the file says `TODO 1`:

```c
GError* vdo_error = NULL;
VdoStream* vdo_stream = NULL;

signal(SIGTERM, shutdown);
signal(SIGINT, shutdown);

openlog(APP_NAME, LOG_PID | LOG_CONS, LOG_USER);

double vdo_stream_framerate = 30.0;
unsigned int vdo_channel = 1;
```

The signal handlers let the app stop cleanly while running the poll loop.

## Step 4: Create the VDO stream and get its fd

Paste this where the file says `TODO 2`:

```c
vdo_stream = create_new_vdo_stream(vdo_channel, vdo_stream_framerate);
if (!vdo_stream) {
    return handle_vdo_failed(vdo_error);
}

int fd = vdo_stream_get_fd(vdo_stream, &vdo_error);
if (fd < 0) {
    return handle_vdo_failed(vdo_error);
}

struct pollfd fds = {
    .fd = fd,
    .events = POLL_IN,
};
```

The helper creates a non-blocking YUV stream at the requested resolution and buffer count.

## Step 5: Start the stream and log metadata

Paste this where the file says `TODO 3`:

```c
log_stream_info(vdo_stream);

if (!vdo_stream_start(vdo_stream, &vdo_error)) {
    return handle_vdo_failed(vdo_error);
}

syslog(LOG_INFO, "Start fetching video frames from VDO");
```

`log_stream_info()` records width, height, pitch, format, buffer type, and buffer count.

## Step 6: Poll and fetch buffers

Paste this where the file says `TODO 4`:

```c
while (running) {
    int status = 0;
    do {
        status = poll(&fds, 1, -1);
    } while (status == -1 && errno == EINTR);

    if (status < 0) {
        panic("Failed to poll with status %d", status);
    }

    VdoBuffer* vdo_buf = vdo_stream_get_buffer(vdo_stream, &vdo_error);
    if (!vdo_buf && g_error_matches(vdo_error, VDO_ERROR, VDO_ERROR_NO_DATA)) {
        g_clear_error(&vdo_error);
        continue;
    }
    if (!vdo_buf) {
        return handle_vdo_failed(vdo_error);
    }
```

The stream is non-blocking, so the app waits on the stream fd before calling `vdo_stream_get_buffer()`.

## Step 7: Inspect the DMA-BUF

Paste this where the file says `TODO 5`:

```c
    inspect_dma_buffer(vdo_buf);
```

The helper logs:

- `fd`
- image `offset`
- mappable `capacity`
- `frame_size`
- the first 32 bytes at the image offset

It maps the fd read-only with `mmap()` and unmaps it before returning.

## Step 8: Return buffers and clean up

Paste this where the file says `TODO 6`:

```c
    if (!vdo_stream_buffer_unref(vdo_stream, &vdo_buf, &vdo_error)) {
        return handle_vdo_failed(vdo_error);
    }
}

syslog(LOG_INFO, "Stopping VDO DMA-BUF example");

if (vdo_stream) {
    g_object_unref(vdo_stream);
}
closelog();
return EXIT_SUCCESS;
```

Do not use `vdo_buf`, the mapped pointer, or frame metadata after `vdo_stream_buffer_unref()`. VDO may immediately reuse the buffer.

## Build

From this example directory:

```sh
docker build --tag vdo-dma-bufs --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-dma-bufs):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `vdo/vdo-dma-bufs` in `axis-acap-tip-workshop`.
