# Test Draw Views

Use this guide after building, installing, and starting the `draw_views` app.

## What to test

The app should draw overlay shapes on the configured views. The render callback decides what to draw based on the stream camera/view data.

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `Draw Views`.
3. Open a live view layout that shows multiple views.
4. Confirm that the rectangle, circle, or triangle overlays appear on the expected views.
5. Stop the app.
6. Confirm that the overlays are removed.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=draw_views"
```

The log should show stream camera/view details from the render callback.

## Troubleshooting

If no shapes are visible, confirm that:

- the app is running
- the manifest contains the `resources` block from the README
- the Makefile links `cairo`
- the live view layout includes the targeted views
- the palette colors are configured before creating the overlay
- `axoverlay_redraw()` is called after the overlay is created
