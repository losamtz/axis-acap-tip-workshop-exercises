# Test Larod Client

Use this guide after building, installing, and starting the `larod_client_tool` app.

## What to test

The app should start and write a syslog message.

## Check logs

Open the app logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=larod_client_tool"
```

Confirm that the log contains:

```text
Larod client tool to enable ssh developer mode, started!
```

Helper scripts for manual larod client experiments are in `.test/tools/`.
