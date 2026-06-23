# Test Object Detection Min

Use this guide after building, installing, and starting the `object_detection_min` app.

## What to test

The app should run camera frames through larod inference, parse SSD outputs, and draw detections with bbox.

## Check logs

Open the app logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=object_detection_min"
```

Confirm that the logs show:

- connected to larod
- model loaded
- four output tensors allocated and mmaped
- VDO stream details
- preprocessing decision
- entering inference loop

## Check live view

Open live view for channel 1 and confirm that detected objects are drawn as bbox overlays when confidence is above the configured threshold.
