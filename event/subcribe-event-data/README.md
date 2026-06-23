# Subcribe Event Data Exercise

This exercise is based on the corresponding complete example in `axis-acap-tip-workshop`.
The source file `app/subscribe_event_data.c` has been reduced to a small TODO skeleton.

Your task is to rebuild the application flow by pasting the snippet below into `app/subscribe_event_data.c`.
The snippet is intentionally kept in the README so you can read the sequence before editing the C file.

## What to do

1. Open `app/subscribe_event_data.c`.
2. Replace the skeleton implementation with the code from **Implementation snippet** below.
3. Read through the code and identify the API setup, runtime loop, and cleanup flow.
4. Build the package with the commands in **Build**.


## Implementation snippet

Paste this into `app/subscribe_event_data.c`:

```c
/**
 * Copyright (C) 2021, Axis Communications AB, Lund, Sweden
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * - subscribe_to_event.c -
 *
 * This example illustrates how to setup an subscription to
 * an ACAP event. => SendData service / SendDataEvent
 *
 * Error handling has been omitted for the sake of brevity.
 */

#include <axsdk/axevent.h>
#include <glib-object.h>
#include <glib.h>
#include <syslog.h>

/**
 * brief Callback function which is called when event subscription is completed.
 *
 * This callback will be called when the subscription has been registered
 * with the event system. The event subscription can now be used for
 * subscribing to events.
 *
 * param subscription Subscription id.
 * param event Subscribed event.
 * param token Token as user data to the callback function.
 */
static void subscription_callback(guint subscription, AXEvent* event, gpointer *userdata) {
    const AXEventKeyValueSet* key_value_set;
    gdouble temperature = 0.0;
    gdouble load = 0.0;
    gint used_memory = 0;
    gint free_memory = 0;

    // The subscription id is not used in this example
    (void)subscription;
    (void)userdata;

    // Extract the AXEventKeyValueSet from the event
    key_value_set = ax_event_get_key_value_set(event);

    // Get the Value of the data event
    ax_event_key_value_set_get_double(key_value_set, "Temperature", NULL, &temperature, NULL);
    ax_event_key_value_set_get_double(key_value_set, "Load", NULL, &load, NULL);
    ax_event_key_value_set_get_integer(key_value_set, "UsedMemory", NULL, &used_memory, NULL);
    ax_event_key_value_set_get_integer(key_value_set, "FreeMemory", NULL, &free_memory, NULL);
    // Print a helpful message
    syslog(LOG_INFO, "Temperature: %f C", temperature);
    syslog(LOG_INFO, "Load: %f", load);
    syslog(LOG_INFO, "Used Memory: %d (MB)", used_memory);
    syslog(LOG_INFO, "Free Memory: %d (MB)", free_memory);

    /*
     * Free the received event, n.b. AXEventKeyValueSet should not be freed
     * since it's owned by the event system until unsubscribing
     */
    ax_event_free(event);
}

/**
 * brief Setup a subscription for an event.
 *
 * Initialize a subscription for AXEventKeyValueSet that matches
 * tnsaxis:SendData/tnsaxis:SendDataEvent, which is using the VAPIX namespace "tnsaxis".
 *
 * Topic: tnsaxis:CameraApplicationPlatform/tnsaxis:SendData/tnsaxis:SendDataEvent
 *
 * param handler Event handler.
 * param token Token as user data to the callback function.
 * return subscription id as integer.
 */
static guint send_data_event_subscription(AXEventHandler* event_handler) {
    AXEventKeyValueSet* key_value_set = NULL;
    guint subscription                = 0;

    key_value_set = ax_event_key_value_set_new();

    // Set keys and namespaces for the event to be subscribed
    ax_event_key_value_set_add_key_value(key_value_set,
                                         "topic0",
                                         "tnsaxis",
                                         "CameraApplicationPlatform",
                                         AX_VALUE_TYPE_STRING,
                                         NULL);
    ax_event_key_value_set_add_key_value(key_value_set,
                                         "topic1",
                                         "tnsaxis",
                                         "SendData",
                                         AX_VALUE_TYPE_STRING,
                                         NULL);
    ax_event_key_value_set_add_key_value(key_value_set,
                                         "topic2",
                                         "tnsaxis",
                                         "SendDataEvent",
                                         AX_VALUE_TYPE_STRING,
                                         NULL);

    /*
     * Time to setup the subscription. Use the "token" input argument as
     * input data to the callback function "subscription callback"
     */
    ax_event_handler_subscribe(event_handler,
                               key_value_set,
                               &subscription,
                               (AXSubscriptionCallback)subscription_callback,
                               NULL,
                               NULL);


    // The key/value set is no longer needed
    ax_event_key_value_set_free(key_value_set);

    return subscription;
}

/**
 * brief Main function which subscribes for an event.
 */
int main(void) {
    GMainLoop* main_loop          = NULL;
    AXEventHandler* event_handler = NULL;
    guint subscription            = 0;
    

    // Set up the user logging to syslog
    openlog(NULL, LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Started logging from subscribe event application");

    // Event handler
    event_handler = ax_event_handler_new();
    subscription  = send_data_event_subscription(event_handler);

    // Main loop
    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);

    // Cleanup event handler
    ax_event_handler_unsubscribe(event_handler, subscription, NULL);
    ax_event_handler_free(event_handler);

    // Free g_main_loop
    g_main_loop_unref(main_loop);
}
```

## Build

From this example directory:

```sh
docker build --tag subcribe-event-data --build-arg ARCH=aarch64 .
docker cp $(docker create subcribe-event-data):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera from the Apps page or with your usual ACAP install flow.
If the application exposes HTTP endpoints or overlays, use the behavior described by the code comments and the parent module README to verify it.

## Reference

The complete version lives in the original `axis-acap-tip-workshop` repository under the same relative path:

`event/subcribe-event-data`
