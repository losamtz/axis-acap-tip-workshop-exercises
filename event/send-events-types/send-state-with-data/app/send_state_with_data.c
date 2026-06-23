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


#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <stdlib.h>
#include <axsdk/axevent.h>
#include <glib-object.h>
#include <glib.h>

#define LOG(fmt, args...)    { syslog(LOG_INFO, fmt, ## args); printf(fmt, ## args); }
#define LOG_ERROR(fmt, args...)    { syslog(LOG_CRIT, fmt, ## args); printf(fmt, ## args); }

typedef struct {
    AXEventHandler* event_handler;
    guint event_id;
    guint timer;
    guint value;
} AppData;

static AppData* app_data = NULL;


/**
 * brief Send event.
 *
 * Send the previously declared event.
 *
 * param send_data Application data containing e.g. the event declaration id.
 * return TRUE
 */
static gboolean send_event(AppData* send_data) {
   AXEventKeyValueSet *key_value_set = NULL;
    AXEvent  *event                   = NULL;
    //GDateTime *time_stamp;
    
    key_value_set = ax_event_key_value_set_new();

    /* TODO 1: Add active state and metadata values to the runtime event. */


    //time_stamp = g_date_time_new_now_local();

    // Create the event
    // Use ax_event_new2 since ax_event_new is deprecated from 3.2
    event = ax_event_new2(key_value_set, NULL);
    
    // The key/value set is no longer needed
    ax_event_key_value_set_free(key_value_set);

    /* TODO 2: Send the state-with-data event. */
    ax_event_free(event);
    //g_date_time_unref(time_stamp);


    send_data->value = !send_data->value;
    
    // Returning TRUE keeps the timer going
    return TRUE;
}


static void declaration_complete(guint declaration, int *value) {
  syslog(LOG_INFO, "Declaration complete for: %d", declaration);

    app_data->value = *value;

    // Set up a timer to be called every 10th second
    app_data->timer = g_timeout_add_seconds(5, (GSourceFunc)send_event, app_data);
}


static guint setup_declaration(AXEventHandler* event_handler, guint *start_value) {
    AXEventKeyValueSet* key_value_set = NULL;
    guint declaration                 = 0;
    GError* error                     = NULL;

    // Create keys, namespaces and nice names for the event
    key_value_set = ax_event_key_value_set_new();

    /* TODO 3: Add the ObjectAnalytics topic keys and event nice name. */

    // marked as data
    //ax_event_key_value_set_mark_as_user_defined( key_value_set, "topic2", "tnsaxis", "isApplicationData", NULL);

    /* TODO 4: Add and mark active plus metadata fields as data. */

    // Declare event
    /* TODO 5: Declare the stateful event with active as the state key. */

    // The key/value set is no longer needed
    ax_event_key_value_set_free(key_value_set);
    return declaration;
}

/**
 * brief Main function which sends an event.
 */

gint main(void) {
    GMainLoop* main_loop = NULL;
    guint start_value = 0;

    openlog("send_state_with_data", LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Started logging from send event application");

    app_data = calloc(1, sizeof(AppData));
    app_data->event_handler = ax_event_handler_new();
    app_data->event_id = setup_declaration(app_data->event_handler, &start_value);

    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);

    ax_event_handler_undeclare(app_data->event_handler, app_data->event_id, NULL);
    ax_event_handler_free(app_data->event_handler);
    free(app_data);
    g_main_loop_unref(main_loop);

    return 0;
}
