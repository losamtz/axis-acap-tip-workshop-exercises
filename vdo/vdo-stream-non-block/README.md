# Vdo Stream Non Block Exercise

This exercise is based on `vdo/vdo-stream-non-block` from the complete `axis-acap-tip-workshop` repository.

`app/vdo_stream_non_block.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Review manifest configuration

This example uses manifest entries for `resources`. Review `app/manifest.json` before building and keep these entries aligned with the README workflow.

## Step 2: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 gio-unix-2.0 vdostream
```

## Step 3: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
g_autoptr(GError) error = NULL;
    g_autoptr(VdoMap) settings = NULL;
    g_autoptr(VdoStream) stream = NULL;
    g_autoptr(VdoMap) info = NULL;

    syslog(LOG_INFO, "Starting %s", argv[0]);

    settings = vdo_map_new();

    // Stream from first channel & Non-blocking socket
    vdo_map_set_uint32(settings, "channel", 1u);
    vdo_map_set_uint32(settings, "format", VDO_FORMAT_H264);
    vdo_map_set_boolean(settings, "socket.blocking", FALSE);

    // Create stream
    stream = vdo_stream_new(settings, NULL, &error);

    if (!stream)
        return handle_vdo_failed(error);
```

## Step 4: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
//Get stream fd
    int fd = vdo_stream_get_fd(stream, &error);

    if (fd < 0)
        return handle_vdo_failed(error);

    struct pollfd fds = {
        .fd = fd,
        .events = POLL_IN,
    };

    // Start stream
    if (!vdo_stream_start(stream, &error))
        return handle_vdo_failed(error);

    syslog(LOG_INFO, "Polling stream fd for new frames with non-blocking socket");

    info = vdo_stream_get_info(stream, &error);

    if (!info)
        panic("%s: Failed to get vdo stream info: %s", __func__, error->message);

    syslog(LOG_INFO, "Starting stream resolution: %ux%u, at %u fps\n", vdo_map_get_uint32(info, "width", 0), vdo_map_get_uint32(info, "height", 0), (unsigned int)(vdo_map_get_double(info, "framerate", 0.0) + 0.5));
```

## Step 5: Add main runtime flow snippet

Paste this into `main()` at the next TODO position:

```c
// Fetch 10 frames
    for (size_t i = 0; i < 10;)
    {
        int status = 0;
        do {
            // If poll returns -1 then errno is set
            // if the errno is set to EINTR then just
            // continue this loop
            status = poll(&fds, 1, -1);
        } while (status == -1 && errno == EINTR);

        if (status < 0) {
            panic("Failed to poll with status %d", status);
        }

        g_autoptr(VdoBuffer) vdo_buf = vdo_stream_get_buffer(stream, &error);

        if (!vdo_buf && g_error_matches(error, VDO_ERROR, VDO_ERROR_NO_DATA)) {
            g_clear_error(&error);
            continue;
        }
        if (!vdo_buf) {
            return handle_vdo_failed(error);
        }
```

## Step 6: Add main processing loop snippet

Paste this into `main()` at the next TODO position:

```c
// Get frame metadata
        VdoFrame *frame = vdo_buffer_get_frame(vdo_buf);

        // Capture timestamp (microseconds)
        gint64 pts = vdo_frame_get_timestamp(frame);

        syslog(LOG_INFO, "<6>Timestamp: %u us - Frame: %u, Size: %zu\n", (unsigned int)pts, vdo_frame_get_sequence_nbr(frame), vdo_frame_get_size(frame));

        // Allow VDO to reuse frame
        if (!vdo_stream_buffer_unref(stream, &vdo_buf, &error))
        {
            return handle_vdo_failed(error);
        }
        // Only successful frames count
        i += 1;
    }

    return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag vdo-stream-non-block --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-stream-non-block):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `vdo/vdo-stream-non-block` in `axis-acap-tip-workshop`.
