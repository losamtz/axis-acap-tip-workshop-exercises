# Test Vdo Utilities

Use this guide after building, installing, and starting the `vdo_utilities` app.

## What to test

The app should log VDO discovery information and exit cleanly. It does not start a video stream or fetch frames.

Expected log content includes:

- available video channel IDs
- filtered input channel IDs
- supported native resolutions for channel 1
- current stream rotation
- `Exiting cleanly.`

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `vdo utilities`.
3. Wait for the app to finish.
4. Open the application log.
5. Confirm that channel, resolution, and rotation messages are present.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=vdo_utilities"
```

## Troubleshooting

If the app fails before logging rotation, confirm that:

- the Makefile links `vdostream`
- the camera has video channel 1
- the requested RGB stream resolution is supported or adjusted by VDO
- the app log does not show an expected maintenance or rotation-related VDO error
