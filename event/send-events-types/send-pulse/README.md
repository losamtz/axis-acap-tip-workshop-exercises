# Send Pulse Exercise

This exercise declares and sends a stateless pulse-style event every five seconds. The event includes a `value` data field so it is visible and selectable in the camera event/action-rule UI.

`app/send_pulse.c` keeps the main loop and event handler lifecycle in place so the exercise can focus on the event declaration and the runtime pulse payload.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 axevent
```

## Step 2: Add the runtime value

Open `app/send_pulse.c` and paste this where the file says `TODO 1`:

```c
ax_event_key_value_set_add_key_value(key_value_set,
                                     "value",
                                     NULL,
                                     &send_data->value,
                                     AX_VALUE_TYPE_INT,
                                     NULL);
```

The value starts at the declaration callback's start value and then changes by ten after each sent event.

## Step 3: Send the event

Paste this where the file says `TODO 2`:

```c
if (!ax_event_handler_send_event(send_data->event_handler,
                                 send_data->event_id,
                                 event,
                                 NULL)) {
    LOG_ERROR("Could not fire event\n");
}

syslog(LOG_INFO, "Value sent: %u", send_data->value);
```

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

The topic path is `CameraApplicationPlatform/SendPulse/SendPulseEvent`.

## Step 5: Add the pulse data field

Paste this where the file says `TODO 4`:

```c
ax_event_key_value_set_add_key_value(key_value_set,
                                     "value",
                                     NULL,
                                     start_value,
                                     AX_VALUE_TYPE_INT,
                                     NULL);
ax_event_key_value_set_mark_as_data(key_value_set, "value", NULL, NULL);
```

Pulse events do not always need data, but this value makes the event visible and selectable in the camera UI.

## Step 6: Declare the stateless event

Paste this where the file says `TODO 5`:

```c
if (!ax_event_handler_declare(event_handler,
                              key_value_set,
                              1,
                              &declaration,
                              (AXDeclarationCompleteCallback)declaration_complete,
                              start_value,
                              NULL)) {
    LOG_ERROR("Could not declare event\n");
}
```

The third argument is `1`, which declares a stateless event. Do not read `error->message` here unless you also pass a valid `GError**` to the declaration call.

## Build

```sh
docker build --tag send-pulse --build-arg ARCH=aarch64 .
docker cp $(docker create send-pulse):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
