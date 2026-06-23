# Send State With Data Exercise

This exercise declares a stateful event with `active` as the state key and additional metadata fields.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 axevent gio-2.0 libcurl jansson
```

## Step 2: Add runtime state and metadata

Open `app/send_state_with_data.c`.

For `TODO 1`, add `active`, `triggerTime`, `classTypes`, `scenarioType`, and `objectId` to the runtime event. For `TODO 2`, send the event with `ax_event_handler_send_event(...)`.

## Step 3: Declare the state and metadata schema

For `TODO 3`, add the `CameraApplicationPlatform/ObjectAnalytics/SendStateWithDataEvent` topic keys.

For `TODO 4`, add and mark these fields as data: `active`, `triggerTime`, `classTypes`, `scenarioType`, and `objectId`.

For `TODO 5`, declare with `ax_event_handler_declare2(...)`, using `FALSE` for stateful behavior and `"active"` as the state key.

## Build

```sh
docker build --tag send-state-with-data --build-arg ARCH=aarch64 .
docker cp $(docker create send-state-with-data):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
