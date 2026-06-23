# Vdo Dma Bufs Exercise

This exercise is based on `vdo/vdo-dma-bufs` from the complete `axis-acap-tip-workshop` repository.

`app/vdo_dma_bufs.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

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
g_autoptr(GError) vdo_error = NULL;
    g_autoptr(VdoStream) vdo_stream = NULL;
    // Stop main loop at signal
    signal(SIGTERM, shutdown);
    signal(SIGINT, shutdown);

    openlog(APP_NAME, LOG_PID | LOG_CONS, LOG_USER);
    // Set to a framerate that is sufficient for inference
    double vdo_stream_framerate = 30.0;
    // The vdo channel to be used
    unsigned int vdo_channel = 1;

    vdo_stream = create_new_vdo_stream(vdo_channel,
                                       vdo_stream_framerate);
    if (!vdo_stream) {
        return handle_vdo_failed(vdo_error);
    }
    int fd = vdo_stream_get_fd(vdo_stream, &vdo_error);
    if (fd < 0) {
        return handle_vdo_failed(vdo_error);
    }
    log_stream_info(vdo_stream);

    struct pollfd fds = {
        .fd     = fd,
        .events = POLL_IN,
    };

    if (!vdo_stream_start(vdo_stream, &vdo_error)) {
        return handle_vdo_failed(vdo_error);
    }
    syslog(LOG_INFO, "Start fetching video frames from VDO");
    printf("Stream started\n");
```

## Step 4: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
    while (running) {
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

        VdoBuffer* vdo_buf = vdo_stream_get_buffer(vdo_stream, &vdo_error);
        if (!vdo_buf && g_error_matches(vdo_error, VDO_ERROR, VDO_ERROR_NO_DATA)) {
            g_clear_error(&vdo_error);
            continue;
        }
        if (!vdo_buf) {
            return handle_vdo_failed(vdo_error);
        }

        if (vdo_buf) {

            inspect_dma_buffer(vdo_buf);
            /*
             * Return the buffer to VDO. Do not read the mapped pointer or
             * VdoBuffer after this point; VDO may reuse the buffer for a new
             * frame immediately.
             */
            if (!vdo_stream_buffer_unref(vdo_stream, &vdo_buf, &vdo_error)) {
                return handle_vdo_failed(vdo_error);
            }
        }
    }

    printf("Stopping...\n");

    g_object_unref(vdo_stream);

    return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag vdo-dma-bufs --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-dma-bufs):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `vdo/vdo-dma-bufs` in `axis-acap-tip-workshop`.
