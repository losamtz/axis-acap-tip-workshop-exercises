# Test Draw Rectangle

Use this guide after building, installing, and starting the `draw_rectangle` app.

## What to test

The app should draw a centered rectangle over video channel 1.

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `Draw Rectangle`.
3. Open the live view for video channel 1.
4. Confirm that a rectangle is visible over the video image.
5. Stop the app.
6. Confirm that the overlay is removed.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=draw_rectangle"
```

The log should show the max resolution and render callback messages.

## Troubleshooting

If no rectangle is visible, confirm that:

- the app is running
- the manifest contains the `resources` block from the README
- the Makefile links `cairo`
- `AXOVERLAY_CAIRO_IMAGE_BACKEND` is supported
- `axoverlay_redraw()` is called after the overlay is created
