# Send Pulse Exercise

This exercise declares and sends a stateless pulse-style event every five seconds.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 axevent
```

## Step 2: Add and send the runtime value

Open `app/send_pulse.c`.

For `TODO 1`, add the current value to the event key/value set:

```c
ax_event_key_value_set_add_key_value(key_value_set, "value", NULL, &send_data->value, AX_VALUE_TYPE_INT, NULL);
```

For `TODO 2`, send the event:

```c
if (!ax_event_handler_send_event(send_data->event_handler, send_data->event_id, event, NULL))
    LOG_ERROR("Could not fire event\n");
```

## Step 3: Declare the pulse event

For `TODO 3`, add `topic0`, `topic1`, and `topic2` using `TOPIC0_TAG`, `TOPIC1_TAG`, and `EVENT_TAG`.

For `TODO 4`, add `value` and mark it as data:

```c
ax_event_key_value_set_add_key_value(key_value_set, "value", NULL, &start_value, AX_VALUE_TYPE_INT, NULL);
ax_event_key_value_set_mark_as_data(key_value_set, "value", NULL, NULL);
```

For `TODO 5`, declare the event as stateless:

```c
if (!ax_event_handler_declare(event_handler, key_value_set, 1, &declaration, (AXDeclarationCompleteCallback)declaration_complete, &start_value, NULL)) {
    syslog(LOG_WARNING, "Could not declare: %s", error->message);
    g_error_free(error);
}
```

## Build

```sh
docker build --tag send-pulse --build-arg ARCH=aarch64 .
docker cp $(docker create send-pulse):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
