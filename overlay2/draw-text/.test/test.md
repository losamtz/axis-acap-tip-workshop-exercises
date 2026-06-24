# Test Overlay2 Draw Text

Use this guide after building, installing, and starting the `overlay2_draw_text` app.

## What to test

The app should start `axoverlay2`, listen for VDO overlay stream events, create an overlay for each matching stream, render countdown text with Cairo, copy the ARGB pixels into an overlay buffer, and submit an updated buffer once per second.

Expected behavior:

- the app starts without errors
- countdown text appears in the video stream
- the text changes once per second
- the text color changes as the countdown value changes
- the app stops cleanly

Expected log content includes:

- `Created overlay`
- no repeated `Failed to get buffer`
- no `Failed to submit overlay buffer`

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `overlay2 draw text`.
3. Open the live view.
4. Confirm that countdown text is visible.
5. Watch the text for at least 10 seconds and confirm that it updates.
6. Stop the app.
7. Confirm that the text disappears.
8. Open the application log and check for overlay or font/cache errors.

## Check logs

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=overlay2_draw_text"
```

## Troubleshooting

If the text is not visible, confirm that:

- the manifest contains the `resources` block from the README
- the Makefile links `axoverlay2`, `vdostream`, `cairo`, and `gio-unix-2.0`
- the app is still running
- the live view stream is overlay-capable
- the log contains `Created overlay`
- `XDG_CACHE_HOME` points to the package localdata directory
- no expected `AXO_ERR_NO_STREAM` or `AXO_ERR_WAIT` condition is being treated as fatal
