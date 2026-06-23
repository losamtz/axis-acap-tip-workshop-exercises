# Send Data Exercise

This exercise declares and sends a stateless event with application data fields.

`app/send_data.c` keeps the main loop and event handler lifecycle in place so the exercise can focus on the event schema and payload.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 axevent
```

## Step 2: Add runtime data values

Open `app/send_data.c` and paste this where the file says `TODO 1`:

```c
if (!ax_event_key_value_set_add_key_value(key_value_set, "Temperature", NULL, &app_data->temperature, AX_VALUE_TYPE_DOUBLE, NULL))
    syslog(LOG_WARNING, "Could not add temperature key/value pair");
if (!ax_event_key_value_set_add_key_value(key_value_set, "Load", NULL, &app_data->load, AX_VALUE_TYPE_DOUBLE, NULL))
    syslog(LOG_WARNING, "Could not add load key/value pair");
if (!ax_event_key_value_set_add_key_value(key_value_set, "UsedMemory", NULL, &app_data->used_memory, AX_VALUE_TYPE_INT, NULL))
    syslog(LOG_WARNING, "Could not add used memory key/value pair");
if (!ax_event_key_value_set_add_key_value(key_value_set, "FreeMemory", NULL, &app_data->free_memory, AX_VALUE_TYPE_INT, NULL))
    syslog(LOG_WARNING, "Could not add free memory key/value pair");
```

## Step 3: Send the event

Paste this where the file says `TODO 2`:

```c
if (!ax_event_handler_send_event(send_data->event_handler, send_data->event_id, event, NULL))
    LOG_ERROR("Could not fire event\n");
else
    LOG("sent data event");
```

## Step 4: Declare the schema

Paste this where the file says `TODO 3`:

```c
ax_event_key_value_set_add_key_value(key_value_set, "topic0", "tnsaxis", TOPIC0_TAG, AX_VALUE_TYPE_STRING, NULL);
ax_event_key_value_set_add_key_value(key_value_set, "topic1", "tnsaxis", TOPIC1_TAG, AX_VALUE_TYPE_STRING, NULL);
ax_event_key_value_set_add_nice_names(key_value_set, "topic1", "tnsaxis", TOPIC1_TAG, TOPIC1_NAME, NULL);
ax_event_key_value_set_add_key_value(key_value_set, "topic2", "tnsaxis", EVENT_TAG, AX_VALUE_TYPE_STRING, NULL);
ax_event_key_value_set_add_nice_names(key_value_set, "topic2", "tnsaxis", EVENT_TAG, EVENT_NAME, NULL);
ax_event_key_value_set_mark_as_user_defined(key_value_set, "topic2", "tnsaxis", "isApplicationData", NULL);
```

Then add each data field with `ax_event_key_value_set_add_key_value(...)`, mark it with `ax_event_key_value_set_mark_as_data(...)`, and mark it as `"isApplicationData"`.

## Step 5: Declare the event

Paste this where the file says `TODO 4`:

```c
if (!ax_event_handler_declare(event_handler, key_value_set, 1, &declaration, (AXDeclarationCompleteCallback)declaration_complete, &start_value, NULL))
    LOG_ERROR("Could not declare event\n");
```

## Build

```sh
docker build --tag send-data --build-arg ARCH=aarch64 .
docker cp $(docker create send-data):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
