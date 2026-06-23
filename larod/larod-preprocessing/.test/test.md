# Test Larod Preprocessing

Use this guide after building, installing, and starting the `larod_preprocessing` app.

## What to test

The app should connect to larod, load the inference model, inspect the VDO stream, decide whether preprocessing is needed, and run inference.

## Check logs

Open the app logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=larod_preprocessing"
```

Confirm that the logs show:

- connected to larod
- model loaded
- model input size and pitch
- VDO stream size, pitch, and format
- whether preprocessing is `YES` or `NO`
- inference loop activity

If preprocessing is expected but not used, compare the VDO format/size with the model input requirements.
