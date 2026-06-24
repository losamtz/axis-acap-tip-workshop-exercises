# Test Overlay2 Draw Views

Use this guide after building, installing, and starting the `overlay2_draw_views` app.

## What to test

The app should start `axoverlay2`, listen for VDO overlay stream events, create one overlay for each matching stream, read the stream `camera` value as `view_id`, draw a different Cairo shape for each view group, copy the ARGB pixels into an overlay buffer, and submit the buffer repeatedly.

Expected behavior:

- the app starts without errors
- an overlay shape appears in the video stream
- different views or camera streams can show different shapes
- the app stops cleanly and the overlay disappears

Expected log content includes:

- `Created overlay`
- no repeated `Failed to get buffer`
- no `Failed to submit overlay buffer`
- no invalid VDO stream size messages

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `overlay2 draw views`.
3. Open the live view.
4. Confirm that an overlay shape is visible.
5. If the device has multiple views or sensors, switch views and confirm that the shape can differ.
6. Stop the app.
7. Confirm that the overlay disappears.
8. Open the application log and check for VDO stream or overlay errors.

## Check logs

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=overlay2_draw_views"
```

## Troubleshooting

If no shape is visible, confirm that:

- the manifest contains the `resources` block from the README
- the Makefile links `axoverlay2`, `vdostream`, `cairo`, and `gio-unix-2.0`
- the live view stream is overlay-capable
- the log contains `Created overlay`
- the VDO stream info contains valid width and height
- the stream `camera` value is read and stored as `view_id`
- no expected `AXO_ERR_NO_STREAM` or `AXO_ERR_WAIT` condition is being treated as fatal
