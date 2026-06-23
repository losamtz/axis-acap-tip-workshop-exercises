# Test Larod Basic

Use this guide after building, installing, and starting the `larod_basic` app.

## What to test

The app should connect to larod, load the model, create an RGB VDO stream at the model input size, and run inference on live frames.

## Check logs

Open the app logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=larod_basic"
```

Confirm that the logs show:

- connection to larod
- model loaded successfully
- model input width, height, and pitch
- VDO stream started
- inference loop activity

If the app fails to start, confirm that the manifest includes `video` group access and `deepLearningProcessor`.
