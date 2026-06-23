# Send State Exercise

This exercise declares a stateful event with an `active` boolean property.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 axevent
```

## Step 2: Add and send the active state

Open `app/send_state.c`.

For `TODO 1`, add `active` to the runtime event. For `TODO 2`, send the event with `ax_event_handler_send_event(...)`.

## Step 3: Declare the stateful event

For `TODO 3`, add the `SendState` topic keys and nice names.

For `TODO 4`, add and mark the `active` bool as data:

```c
ax_event_key_value_set_add_key_value(key_value_set, "active", NULL, &start_value, AX_VALUE_TYPE_BOOL, NULL);
ax_event_key_value_set_mark_as_data(key_value_set, "active", NULL, NULL);
```

For `TODO 5`, declare the event as stateful by passing `0` as the state flag:

```c
if (!ax_event_handler_declare(event_handler, key_value_set, 0, &declaration, (AXDeclarationCompleteCallback)declaration_complete, &start_value, NULL)) {
    syslog(LOG_WARNING, "Could not declare: %s", error->message);
    g_error_free(error);
}
```

## Build

```sh
docker build --tag send-state --build-arg ARCH=aarch64 .
docker cp $(docker create send-state):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
