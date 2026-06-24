# Send Data Exercise

This exercise declares and sends a stateless event with application data fields. The event is marked as application data, so it is meant for clients that know how to consume the payload rather than for general action-rule selection.

`app/send_data.c` keeps the main loop and event handler lifecycle in place so the exercise can focus on the event schema and payload.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 axevent
```

## Step 2: Add runtime data values

Open `app/send_data.c` and paste this where the file says `TODO 1`:

```c
if (!ax_event_key_value_set_add_key_value(key_value_set,
                                          "Temperature",
                                          NULL,
                                          &send_data->temperature,
                                          AX_VALUE_TYPE_DOUBLE,
                                          NULL)) {
    syslog(LOG_WARNING, "Could not add temperature key/value pair");
}

if (!ax_event_key_value_set_add_key_value(key_value_set,
                                          "Load",
                                          NULL,
                                          &send_data->load,
                                          AX_VALUE_TYPE_DOUBLE,
                                          NULL)) {
    syslog(LOG_WARNING, "Could not add load key/value pair");
}

if (!ax_event_key_value_set_add_key_value(key_value_set,
                                          "UsedMemory",
                                          NULL,
                                          &send_data->used_memory,
                                          AX_VALUE_TYPE_INT,
                                          NULL)) {
    syslog(LOG_WARNING, "Could not add used memory key/value pair");
}

if (!ax_event_key_value_set_add_key_value(key_value_set,
                                          "FreeMemory",
                                          NULL,
                                          &send_data->free_memory,
                                          AX_VALUE_TYPE_INT,
                                          NULL)) {
    syslog(LOG_WARNING, "Could not add free memory key/value pair");
}
```

Use the `send_data` function argument here. It is the current `AppData` instance passed by the timer.

## Step 3: Send the event

Paste this where the file says `TODO 2`:

```c
if (!ax_event_handler_send_event(send_data->event_handler,
                                 send_data->event_id,
                                 event,
                                 NULL)) {
    LOG_ERROR("Could not fire event\n");
} else {
    LOG("sent data event");
}
```

## Step 4: Declare the schema

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
ax_event_key_value_set_mark_as_user_defined(key_value_set,
                                            "topic2",
                                            "tnsaxis",
                                            "isApplicationData",
                                            NULL);

ax_event_key_value_set_add_key_value(key_value_set,
                                     "Temperature",
                                     NULL,
                                     &temperature,
                                     AX_VALUE_TYPE_DOUBLE,
                                     NULL);
ax_event_key_value_set_mark_as_data(key_value_set, "Temperature", NULL, NULL);
ax_event_key_value_set_mark_as_user_defined(key_value_set,
                                            "Temperature",
                                            NULL,
                                            "isApplicationData",
                                            NULL);

ax_event_key_value_set_add_key_value(key_value_set,
                                     "Load",
                                     NULL,
                                     &load,
                                     AX_VALUE_TYPE_DOUBLE,
                                     NULL);
ax_event_key_value_set_mark_as_data(key_value_set, "Load", NULL, NULL);
ax_event_key_value_set_mark_as_user_defined(key_value_set,
                                            "Load",
                                            NULL,
                                            "isApplicationData",
                                            NULL);

ax_event_key_value_set_add_key_value(key_value_set,
                                     "UsedMemory",
                                     NULL,
                                     &used_memory,
                                     AX_VALUE_TYPE_INT,
                                     NULL);
ax_event_key_value_set_add_nice_names(key_value_set,
                                      "UsedMemory",
                                      NULL,
                                      "UsedMemory",
                                      "Used Memory (MB)",
                                      NULL);
ax_event_key_value_set_mark_as_data(key_value_set, "UsedMemory", NULL, NULL);
ax_event_key_value_set_mark_as_user_defined(key_value_set,
                                            "UsedMemory",
                                            NULL,
                                            "isApplicationData",
                                            NULL);

ax_event_key_value_set_add_key_value(key_value_set,
                                     "FreeMemory",
                                     NULL,
                                     &free_memory,
                                     AX_VALUE_TYPE_INT,
                                     NULL);
ax_event_key_value_set_add_nice_names(key_value_set,
                                      "FreeMemory",
                                      NULL,
                                      "FreeMemory",
                                      "Free Memory (MB)",
                                      NULL);
ax_event_key_value_set_mark_as_data(key_value_set, "FreeMemory", NULL, NULL);
ax_event_key_value_set_mark_as_user_defined(key_value_set,
                                            "FreeMemory",
                                            NULL,
                                            "isApplicationData",
                                            NULL);
```

The topic path is `CameraApplicationPlatform/SendData/SendDataEvent`. The payload fields are marked as data and as application data.

## Step 5: Declare the event

Paste this where the file says `TODO 4`:

```c
if (!ax_event_handler_declare(event_handler,
                              key_value_set,
                              1,
                              &declaration,
                              (AXDeclarationCompleteCallback)declaration_complete,
                              &start_value,
                              NULL)) {
    LOG_ERROR("Could not declare event\n");
}
```

The third argument is `1`, which declares a stateless event.

## Build

```sh
docker build --tag send-data --build-arg ARCH=aarch64 .
docker cp $(docker create send-data):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
