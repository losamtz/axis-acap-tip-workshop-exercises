# Test Overlay2 Add Logo

Use this guide after building, installing, and starting the `overlay2_add_logo` app.

## What to test

The app should start `axoverlay2`, listen for VDO overlay stream events, create an overlay for each matching stream, load the packaged `axis_tip_logo.png`, draw it in the top-right corner with Cairo, copy the ARGB pixels into an overlay buffer, and submit the buffer repeatedly.

Expected behavior:

- the app starts without errors
- the Axis TIP logo appears in the top-right corner of the video stream
- the logo remains visible while the app is running
- the app stops cleanly and the logo disappears

Expected log content includes:

- `Created overlay`
- no `Failed to load logo image`
- no repeated `Failed to get buffer`
- no `Failed to submit overlay buffer`

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `overlay2 add logo`.
3. Open the live view.
4. Confirm that the logo is visible in the top-right corner.
5. Stop the app.
6. Confirm that the logo disappears.
7. Open the application log and check for PNG loading or overlay errors.

## Check logs

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=overlay2_add_logo"
```

## Troubleshooting

If the logo is not visible, confirm that:

- the manifest contains the `resources` block from the README
- the Makefile links `axoverlay2`, `vdostream`, `cairo`, and `gio-unix-2.0`
- `axis_tip_logo.png` is included in the application package
- the code loads `/usr/local/packages/overlay2_add_logo/axis_tip_logo.png`
- the live view stream is overlay-capable
- the log contains `Created overlay`
- no expected `AXO_ERR_NO_STREAM` or `AXO_ERR_WAIT` condition is being treated as fatal
