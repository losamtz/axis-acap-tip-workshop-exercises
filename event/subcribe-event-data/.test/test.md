# Test Subscribe Event Data

Use this guide after building, installing, and starting both `send_data` and `subscribe_event_data`.

## What to test

The subscriber should receive `SendDataEvent` events and log these fields:

- `Temperature`
- `Load`
- `UsedMemory`
- `FreeMemory`

## Test flow

1. Start `send_data`.
2. Start `subscribe_event_data`.
3. Open the subscriber app logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=subscribe_event_data"
```

4. Confirm that values are logged every time `send_data` publishes an event.

If no events arrive, confirm that the subscriber topic keys match `CameraApplicationPlatform/SendData/SendDataEvent`.
