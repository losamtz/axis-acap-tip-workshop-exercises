# Test Bbox View

Use this guide after building, installing, and starting the `bbox_view` app.

## What to test

The app should draw a thin red rectangle near the edges of the video image on video channel 1.

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `Bbox View`.
3. Open the live view for video channel 1.
4. Confirm that a red outline rectangle is visible over the video image.
5. Stop the app.
6. Confirm that the rectangle is removed from the video image.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=bbox_view"
```

When the app is stopped with `SIGTERM` or `SIGINT`, the log should contain:

```text
Application was stopped by SIGTERM or SIGINT.
```

## Troubleshooting

If no rectangle is visible, confirm that:

- the app is running
- the manifest contains the `resources` block from the README
- the Makefile links the `bbox` package
- live view is showing video channel 1
