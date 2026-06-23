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

    /* TODO 1: Extract the data fields from the received event. */
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

    /* TODO 2: Add the SendData topic keys to the subscription filter. */

    /*
     * Time to setup the subscription. Use the "token" input argument as
     * input data to the callback function "subscription callback"
     */
    /* TODO 3: Subscribe with the callback. */


    // The key/value set is no longer needed
    ax_event_key_value_set_free(key_value_set);

    return subscription;
}

/**
 * brief Main function which subscribes for an event.
 */

int main(void) {
    GMainLoop* main_loop = NULL;
    AXEventHandler* event_handler = NULL;
    guint subscription = 0;

    openlog(NULL, LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Started logging from subscribe event application");

    event_handler = ax_event_handler_new();
    subscription = send_data_event_subscription(event_handler);

    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);

    ax_event_handler_unsubscribe(event_handler, subscription, NULL);
    ax_event_handler_free(event_handler);
    g_main_loop_unref(main_loop);

    return 0;
}
