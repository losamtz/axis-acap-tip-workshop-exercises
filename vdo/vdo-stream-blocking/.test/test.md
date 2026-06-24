# Test Vdo Stream Blocking

Use this guide after building, installing, and starting the `vdo_stream_blocking` app.

## What to test

The app should create an H.264 VDO stream, fetch 10 frames with the blocking `vdo_stream_get_buffer()` call, log frame metadata, return each buffer, and exit.

Expected log content includes:

- stream resolution and framerate
- 10 frame metadata lines with timestamp, frame number, and size

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `VDO stream blocking`.
3. Wait for the app to finish.
4. Open the application log.
5. Confirm that 10 H.264 frame messages were logged.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=vdo_stream_blocking"
```

## Troubleshooting

If the app does not log frames, confirm that:

- the manifest contains the `resources` block from the README
- the Makefile links `vdostream`
- video channel 1 exists
- the app is allowed to access the `video` group
- no expected maintenance or rotation-related VDO error is reported
