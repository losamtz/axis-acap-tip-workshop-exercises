# Test Vdo Larod Min

Use this guide after building, installing, and starting the `vdo_larod_min` app.

## What to test

The app should run a non-blocking VDO-to-larod loop using `poll`, tracked VDO buffers, optional preprocessing, and inference jobs.

## Check logs

Open the app logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=vdo_larod_min"
```

Confirm that the logs show:

- selected inference backend
- model loaded
- VDO stream details
- preprocessing decision
- tracked VDO buffer slots
- inference loop output

If frames do not arrive, confirm that the VDO stream is started and `poll` is waiting on the VDO stream fd.
