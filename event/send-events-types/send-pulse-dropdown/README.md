# Send Pulse Dropdown Exercise

This exercise declares several stateless pulse events where `value` is marked as a source field. Declaring one event per value makes the camera event/action UI show the values as dropdown choices.

`app/send_pulse_drop_down.c` keeps the main loop and event handler lifecycle in place so the exercise can focus on source values, multiple declarations, and sending the event id that matches the selected value.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 axevent
```

## Step 2: Add the selected runtime value

Open `app/send_pulse_drop_down.c` and paste this where the file says `TODO 1`:

```c
ax_event_key_value_set_add_key_value(key_value_set,
                                     "value",
                                     NULL,
                                     &value,
                                     AX_VALUE_TYPE_INT,
                                     NULL);
```

The runtime `value` comes from `send_data->values[send_data->value_index]`.

## Step 3: Send the matching declaration

Paste this where the file says `TODO 2`:

```c
if (!ax_event_handler_send_event(send_data->event_handler,
                                 event_id,
                                 event,
                                 NULL)) {
    LOG_ERROR("Could not fire event with value: %u\n", value);
} else {
    LOG("Event sent with value: %u\n", value);
}
```

Use `event_id`, not a single shared event id. Each dropdown value has its own declaration id.

## Step 4: Declare the topic path

Paste this where the file says `TODO 3`:

```c
ax_event_key_value_set_add_key_value(key_value_set,
                                     "topic0",
                                     "tnsaxis",
                                     TOPIC0_TAG,
                                     AX_VALUE_TYPE_STRING,
                                     NULL);

ax_event_key_value_set_add_key_value(key_value_set,
                                     "topic1",
                                     "tnsaxis",
                                     TOPIC1_TAG,
                                     AX_VALUE_TYPE_STRING,
                                     NULL);
ax_event_key_value_set_add_nice_names(key_value_set,
                                      "topic1",
                                      "tnsaxis",
                                      TOPIC1_TAG,
                                      TOPIC1_NAME,
                                      NULL);

ax_event_key_value_set_add_key_value(key_value_set,
                                     "topic2",
                                     "tnsaxis",
                                     EVENT_TAG,
                                     AX_VALUE_TYPE_STRING,
                                     NULL);
ax_event_key_value_set_add_nice_names(key_value_set,
                                      "topic2",
                                      "tnsaxis",
                                      EVENT_TAG,
                                      EVENT_NAME,
                                      NULL);
```

The topic path is `CameraApplicationPlatform/SendPulseDropDown/SendPulseDropDownEvent`.

## Step 5: Add the dropdown source value

Paste this where the file says `TODO 4`:

```c
ax_event_key_value_set_add_key_value(key_value_set,
                                     "value",
                                     NULL,
                                     value,
                                     AX_VALUE_TYPE_INT,
                                     NULL);
ax_event_key_value_set_mark_as_source(key_value_set, "value", NULL, NULL);
```

Marking `value` as a source field lets the camera UI present the declared values as dropdown choices.

## Step 6: Declare the stateless event

Paste this where the file says `TODO 5`:

```c
if (!ax_event_handler_declare(event_handler,
                              key_value_set,
                              1,
                              &declaration,
                              (AXDeclarationCompleteCallback)declaration_complete,
                              value,
                              NULL)) {
    LOG_ERROR("Could not declare event\n");
}
```

The third argument is `1`, which declares a stateless event. Do not read `error->message` here unless you also pass a valid `GError**` to the declaration call.

## Build

```sh
docker build --tag send-pulse-dropdown --build-arg ARCH=aarch64 .
docker cp $(docker create send-pulse-dropdown):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
