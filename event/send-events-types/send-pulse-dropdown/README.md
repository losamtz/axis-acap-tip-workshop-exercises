# Send Pulse Dropdown Exercise

This exercise declares several stateless pulse events where `value` is marked as a source field, making the values available as dropdown choices in the event/action UI.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 axevent
```

## Step 2: Add and send the selected value

Open `app/send_pulse_drop_down.c`.

For `TODO 1`, add the selected value to the runtime event:

```c
ax_event_key_value_set_add_key_value(key_value_set, "value", NULL, &value, AX_VALUE_TYPE_INT, NULL);
```

For `TODO 2`, send with the declaration id that matches that value:

```c
if (!ax_event_handler_send_event(send_data->event_handler, event_id, event, NULL))
    LOG_ERROR("Could not fire event with value: %u\n", value);
else
    LOG("Event sent with value: %u\n", value);
```

## Step 3: Declare source values

For `TODO 3`, add `topic0`, `topic1`, and `topic2` using the constants in the file.

For `TODO 4`, add `value` and mark it as a source:

```c
ax_event_key_value_set_add_key_value(key_value_set, "value", NULL, value, AX_VALUE_TYPE_INT, NULL);
ax_event_key_value_set_mark_as_source(key_value_set, "value", NULL, NULL);
```

For `TODO 5`, declare the event as stateless.

## Build

```sh
docker build --tag send-pulse-dropdown --build-arg ARCH=aarch64 .
docker cp $(docker create send-pulse-dropdown):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
