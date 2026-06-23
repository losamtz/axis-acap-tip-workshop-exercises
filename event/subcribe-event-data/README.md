# Subscribe Event Data Exercise

This exercise subscribes to the `send-data` event and extracts its payload fields.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 axevent
```

## Step 2: Extract payload fields

Open `app/subscribe_event_data.c` and paste this where the file says `TODO 1`:

```c
ax_event_key_value_set_get_double(key_value_set, "Temperature", NULL, &temperature, NULL);
ax_event_key_value_set_get_double(key_value_set, "Load", NULL, &load, NULL);
ax_event_key_value_set_get_integer(key_value_set, "UsedMemory", NULL, &used_memory, NULL);
ax_event_key_value_set_get_integer(key_value_set, "FreeMemory", NULL, &free_memory, NULL);
```

## Step 3: Match the SendData topic

Paste this where the file says `TODO 2`:

```c
ax_event_key_value_set_add_key_value(key_value_set, "topic0", "tnsaxis", "CameraApplicationPlatform", AX_VALUE_TYPE_STRING, NULL);
ax_event_key_value_set_add_key_value(key_value_set, "topic1", "tnsaxis", "SendData", AX_VALUE_TYPE_STRING, NULL);
ax_event_key_value_set_add_key_value(key_value_set, "topic2", "tnsaxis", "SendDataEvent", AX_VALUE_TYPE_STRING, NULL);
```

## Step 4: Subscribe with the callback

Paste this where the file says `TODO 3`:

```c
ax_event_handler_subscribe(event_handler, key_value_set, &subscription, (AXSubscriptionCallback)subscription_callback, NULL, NULL);
```

## Build

```sh
docker build --tag subcribe-event-data --build-arg ARCH=aarch64 .
docker cp $(docker create subcribe-event-data):/opt/app ./build
```

## Verify

Install this application together with `send-data`, start both, and follow the [test guide](.test/test.md).
