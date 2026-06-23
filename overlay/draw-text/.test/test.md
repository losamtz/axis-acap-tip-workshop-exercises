# Test Draw Text

Use this guide after building, installing, and starting the `draw_text` app.

## What to test

The app should draw text over the video image and redraw it on the timer.

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `Draw Text`.
3. Open the live view for video channel 1.
4. Confirm that text is visible over the video image.
5. Wait a few seconds and confirm that the text redraws.
6. Stop the app.
7. Confirm that the overlay is removed.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=draw_text"
```

The log should show max resolution, render callback messages, and timer-driven redraws.

## Troubleshooting

If no text is visible, confirm that:

- the app is running
- the manifest contains the `resources` block from the README
- the Makefile links `cairo`
- the overlay uses `AXOVERLAY_COLORSPACE_ARGB32`
- the redraw timer calls `axoverlay_redraw()`
