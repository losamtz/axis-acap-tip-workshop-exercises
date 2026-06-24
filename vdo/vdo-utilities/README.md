# Vdo Utilities Exercise

This exercise introduces VDO discovery. It does not fetch frames. It lists available video channels, filtered input channels, supported resolutions, and stream rotation metadata.

`app/vdo_utilities.c` keeps the helper functions in place so the exercise can focus on the order of the VDO discovery calls.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 gio-unix-2.0 vdostream
```

## Step 2: Initialize logging and variables

Open `app/vdo_utilities.c`.

Paste this where the file says `TODO 1`:

```c
GError* error = NULL;
VdoStream* vdo_stream = NULL;

openlog(NULL, LOG_PID, LOG_USER);
syslog(LOG_INFO, "Starting VDO utilities example");
```

This prepares logging and keeps stream ownership explicit.

## Step 3: Discover channels and resolutions

Paste this where the file says `TODO 2`:

```c
get_video_channels();
get_filtered_channels();
get_stream_resolutions();
```

These helpers log the available VDO channels, input channels, and native resolutions for channel 1.

## Step 4: Create a stream for metadata inspection

Paste this where the file says `TODO 3`:

```c
vdo_stream = vdo_stream_rgb_new(NULL,
                                1u,
                                (VdoResolution){.width = WITH, .height = HEIGHT},
                                &error);
if (!vdo_stream) {
    return handle_vdo_failed(error);
}
```

The stream is created but not started. That is enough to inspect stream metadata such as rotation.

## Step 5: Log rotation and clean up

Paste this where the file says `TODO 4`:

```c
get_stream_rotation(vdo_stream);

g_object_unref(vdo_stream);
vdo_stream = NULL;

syslog(LOG_INFO, "Exiting cleanly.");
closelog();
return EXIT_SUCCESS;
```

This reads stream metadata, releases the stream, and exits.

## Build

From this example directory:

```sh
docker build --tag vdo-utilities --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-utilities):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `vdo/vdo-utilities` in `axis-acap-tip-workshop`.
