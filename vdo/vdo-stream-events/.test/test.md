# Test VDO Stream Events

Use this guide after building, installing, and starting the `vdo_stream_events` app.

## What to test

The app should attach an overlay stream-event filter, register the VDO event fd in a GLib main loop, log stream events, and print metadata for existing or newly created overlay-capable streams.

Expected log content includes:

- a startup message
- `Waiting for overlay-capable stream events`
- one or more `VDO stream event` lines
- stream width, height, format, camera, and rotation for `EXISTING` or `CREATED` events
- a shutdown message after the app is stopped

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `vdo stream events`.
3. Let it run for a few seconds.
4. Start or stop another video/overlay-related app if no stream events appear immediately.
5. Stop the app.
6. Open the application log.
7. Confirm that stream events and stream metadata were logged.

## Check logs

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=vdo_stream_events"
```

## Troubleshooting

If no stream events are logged, confirm that:

- the manifest contains the `resources` block from the README
- the Makefile links `vdostream`
- the app has started and is still running
- the event fd was added to the GLib main loop
- another video or overlay-capable stream exists on the device
