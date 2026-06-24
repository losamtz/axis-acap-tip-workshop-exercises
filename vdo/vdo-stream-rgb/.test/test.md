# Test Vdo Stream RGB

Use this guide after building, installing, and starting the `vdo_stream_rgb` app.

## What to test

The app should create a non-blocking RGB VDO stream, wait on the stream fd with `poll()`, fetch 10 RGB frames, log frame metadata, return each buffer, and exit.

Expected log content includes:

- `Starting stream format RGB`
- width, height, pitch, and framerate
- 10 RGB frame metadata lines with timestamp, frame number, and size

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `vdo stream rgb`.
3. Wait for the app to finish.
4. Open the application log.
5. Confirm that 10 RGB frame messages were logged.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=vdo_stream_rgb"
```

## Troubleshooting

If the app does not log RGB frames, confirm that:

- the manifest contains the `resources` block from the README
- the Makefile links `vdostream`
- video channel 1 exists
- the product supports RGB VDO output at the requested resolution
- no expected maintenance or rotation-related VDO error is reported
