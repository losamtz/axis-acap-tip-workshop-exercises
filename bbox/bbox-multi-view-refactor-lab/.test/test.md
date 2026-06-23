# Test Bbox Multi View Refactor Lab

Use this guide after building, installing, and starting the `bbox_multi_view_lab` app.

## What to test

The app should draw a yellow corner-style bounding box on views 1, 2, 3, and 4. The box should move smoothly because the app reuses one persistent bbox handle and lets the GLib timer control the frame cadence.

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `BBox Multi View Refactor Lab`.
3. Open a live view layout that shows views 1, 2, 3, and 4.
4. Confirm that the same yellow box is visible on each targeted view.
5. Confirm that the box moves horizontally without long pauses between frames.
6. Stop the app.
7. Confirm that the overlay is removed.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=bbox_multi_view_lab"
```

When the app is stopped with `SIGTERM` or `SIGINT`, the log should contain:

```text
Stopping (SIGTERM/SIGINT).
```

## Compare with bbox-multi-view

Compared with `bbox_multi_view`, this refactor lab should:

- create the bbox handle once during startup
- reuse the same handle on each animation tick
- avoid `sleep()` in the update loop
- clear and destroy resources on shutdown
