# Test Overlay2 Draw Rectangle

Use this guide after building, installing, and starting the `overlay2_draw_rectangle` app.

## What to test

The app should start `axoverlay2`, listen for VDO overlay stream events, create an overlay for each matching stream, draw a transparent ARGB surface with a yellow rectangle, copy it into an overlay buffer, and submit it repeatedly.

Expected behavior:

- the app starts without errors
- a yellow rectangle appears in the video stream
- the rectangle remains visible while the app is running
- the app stops cleanly

Expected log content includes:

- `Created overlay`
- no repeated `Failed to get buffer`
- no `Failed to submit overlay buffer`

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `overlay2 draw rectangle`.
3. Open the live view.
4. Confirm that a yellow rectangle is visible.
5. Stop the app.
6. Confirm that the rectangle disappears.
7. Open the application log and check for overlay creation errors.

## Check logs

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=overlay2_draw_rectangle"
```

## Troubleshooting

If the rectangle is not visible, confirm that:

- the manifest contains the `resources` block from the README
- the Makefile links `axoverlay2`, `vdostream`, `cairo`, and `gio-unix-2.0`
- the app is still running
- the live view stream is overlay-capable
- the log contains `Created overlay`
- no expected `AXO_ERR_NO_STREAM` or `AXO_ERR_WAIT` condition is being treated as fatal
