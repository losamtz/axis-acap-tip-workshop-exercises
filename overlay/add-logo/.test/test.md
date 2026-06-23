# Test Add Logo

Use this guide after building, installing, and starting the `add_logo` app.

## What to test

The app should draw the packaged AXIS TIP logo over the video image.

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `Add Logo`.
3. Open the live view for video channel 1.
4. Confirm that the logo is visible over the video image.
5. Stop the app.
6. Confirm that the overlay is removed.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=add_logo"
```

The log should show max resolution and render callback messages.

## Troubleshooting

If no logo is visible, confirm that:

- the app is running
- the manifest contains the `resources` block from the README
- the Makefile links `cairo`
- `axis_tip_logo.png` is packaged with the app
- the overlay uses `AXOVERLAY_COLORSPACE_ARGB32`
- `axoverlay_redraw()` is called after the overlay is created
