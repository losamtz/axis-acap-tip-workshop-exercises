# Vdo Utilities Exercise

This exercise is based on `vdo/vdo-utilities` from the complete `axis-acap-tip-workshop` repository.

`app/vdo_utilities.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 gio-unix-2.0 vdostream
```

## Step 2: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
g_autoptr(GError) error = NULL;
    g_autoptr(VdoStream) vdo_stream = NULL;

    openlog(NULL, LOG_PID, LOG_USER);

    // 1- List channels & resolutions
    get_video_channels();
    get_filtered_channels();
    get_stream_resolutions();

    // 3 - create stream without starting it to show how to retrieve stream information such as rotation. No need to start stream for that.
    vdo_stream = vdo_stream_rgb_new(NULL, 1u, (VdoResolution){ .width = WITH, .height = HEIGHT }, &error);

    if (!vdo_stream) {
        return handle_vdo_failed(error);
    }

    // 4 - get rotation
    get_stream_rotation(vdo_stream);

    if (vdo_stream) {
        g_object_unref(vdo_stream);
    }

    syslog(LOG_INFO, "Exiting cleanly.");
    closelog();
    return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag vdo-utilities --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-utilities):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `vdo/vdo-utilities` in `axis-acap-tip-workshop`.
