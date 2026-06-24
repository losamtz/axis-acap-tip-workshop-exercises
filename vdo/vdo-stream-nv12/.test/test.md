# Test Vdo Stream NV12

Use this guide after building, installing, and starting the `vdo_stream_nv12` app.

## What to test

The app should create a non-blocking NV12 VDO stream, wait on the stream fd with `poll()`, fetch 10 NV12 frames, log frame metadata, return each buffer, and exit.

Expected log content includes:

- `Starting stream format NV12`
- width, height, pitch, and framerate
- 10 NV12 frame metadata lines with timestamp, frame number, and size

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `vdo stream nv12`.
3. Wait for the app to finish.
4. Open the application log.
5. Confirm that 10 NV12 frame messages were logged.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=vdo_stream_nv12"
```

## Troubleshooting

If the app does not log NV12 frames, confirm that:

- the manifest contains the `resources` block from the README
- the Makefile links `vdostream`
- video channel 1 exists
- the product supports NV12 VDO output at the requested resolution
- pitch is read from stream info instead of assuming it equals width
- no expected maintenance or rotation-related VDO error is reported
