# Test Bbox Multi View

Use this guide after building, installing, and starting the `bbox_multi_view` app.

## What to test

The app should draw a yellow corner-style bounding box on views 1, 2, 3, and 4. The box should move horizontally and bounce at the left and right edges.

The expected visual behavior is similar to:

[multiview_bbox.mp4](./multiview_bbox.mp4)

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `Bbox Multi View`.
3. Open a live view layout that shows views 1, 2, 3, and 4.
4. Confirm that the same yellow box is visible on each targeted view.
5. Confirm that the box moves horizontally and changes direction at the edges.
6. Stop the app.
7. Confirm that the overlay is removed.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=bbox_multi_view"
```

When the app is stopped with `SIGTERM` or `SIGINT`, the log should contain:

```text
Application was stopped by SIGTERM or SIGINT.
```

## Troubleshooting

If no box is visible, confirm that:

- the app is running
- the manifest contains the `resources` block from the README
- the Makefile links the `bbox` package
- the live view layout includes the targeted views
