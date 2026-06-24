# Send State Exercise

This exercise declares a stateful event with an `active` boolean property. The app sends the current state every five seconds and toggles the value after each send.

`app/send_state.c` keeps the main loop and event handler lifecycle in place so the exercise can focus on the stateful event declaration and runtime state updates.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 axevent
```

## Step 2: Add the runtime state

Open `app/send_state.c` and paste this where the file says `TODO 1`:

```c
ax_event_key_value_set_add_key_value(key_value_set,
                                     "active",
                                     NULL,
                                     &send_data->state_value,
                                     AX_VALUE_TYPE_BOOL,
                                     NULL);
```

The value is sent as the event state and then toggled after the event is sent.

## Step 3: Send the event

Paste this where the file says `TODO 2`:

```c
if (!ax_event_handler_send_event(send_data->event_handler,
                                 send_data->event_id,
                                 event,
                                 NULL)) {
    LOG_ERROR("Could not fire event\n");
}
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

The topic path is `CameraApplicationPlatform/SendState/SendStateEvent`.

## Step 5: Add the active data field

Paste this where the file says `TODO 4`:

```c
ax_event_key_value_set_add_key_value(key_value_set,
                                     "active",
                                     NULL,
                                     start_value,
                                     AX_VALUE_TYPE_BOOL,
                                     NULL);
ax_event_key_value_set_mark_as_data(key_value_set, "active", NULL, NULL);
```

Marking `active` as data tells the event system that this field carries the state value.

## Step 6: Declare the stateful event

Paste this where the file says `TODO 5`:

```c
if (!ax_event_handler_declare(event_handler,
                              key_value_set,
                              0,
                              &declaration,
                              (AXDeclarationCompleteCallback)declaration_complete,
                              start_value,
                              NULL)) {
    LOG_ERROR("Could not declare event\n");
}
```

The third argument is `0`, which declares a stateful event. Do not read `error->message` here unless you also pass a valid `GError**` to the declaration call.

## Build

```sh
docker build --tag send-state --build-arg ARCH=aarch64 .
docker cp $(docker create send-state):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
