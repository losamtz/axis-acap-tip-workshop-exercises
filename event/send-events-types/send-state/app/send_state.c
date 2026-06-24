#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <stdlib.h>
#include <axsdk/axevent.h>
#include <glib-object.h>
#include <glib.h>

#define LOG(fmt, args...)    { syslog(LOG_INFO, fmt, ## args); printf(fmt, ## args); }
#define LOG_ERROR(fmt, args...)    { syslog(LOG_CRIT, fmt, ## args); printf(fmt, ## args); }

#define SERVICE_ID   "send_state"

#define TOPIC0_TAG  "CameraApplicationPlatform"
#define TOPIC0_NAME "ACAP"
#define TOPIC1_TAG  "SendState"
#define TOPIC1_NAME "Send State"
#define EVENT_TAG   "SendStateEvent"
#define EVENT_NAME  "Send State Event"

typedef struct {
    AXEventHandler* event_handler;
    guint event_id;
    guint timer;
    guint state_value;
} AppData;

static AppData* app_data = NULL;


static gboolean send_event(AppData *send_data) {

    AXEventKeyValueSet *key_value_set = NULL;
    AXEvent  *event                   = NULL;
    //GDateTime *time_stamp;
    
    key_value_set = ax_event_key_value_set_new();

    /* TODO 1: Add the current active state value to the runtime event. */
    
    //time_stamp = g_date_time_new_now_local();

    // Create the event
    // Use ax_event_new2 since ax_event_new is deprecated from 3.2
    event = ax_event_new2(key_value_set, NULL);
    
    // The key/value set is no longer needed
    ax_event_key_value_set_free(key_value_set);

    /* TODO 2: Send the state event before toggling the value. */
    ax_event_free(event);
    //g_date_time_unref(time_stamp);


    send_data->state_value = !send_data->state_value;
    
    // Returning TRUE keeps the timer going
    return TRUE;
}
static void declaration_complete(guint declaration, guint *start_value) {
    syslog(LOG_INFO, "Declaration complete for: %d", declaration);
    syslog(LOG_INFO, "Declaration complete start value: %u", *start_value);

    app_data->state_value = *start_value;

    // Set up a timer to be called every 10th second
    app_data->timer = g_timeout_add_seconds(5, (GSourceFunc)send_event, app_data);
}

static guint setup_declaration(AXEventHandler* event_handler, guint *start_value) {

    AXEventKeyValueSet* key_value_set = NULL;
    guint declaration                 = 0;
    key_value_set = ax_event_key_value_set_new();
    
  //Note that the name space is "tnsaxis:".  It is not recommended to create own name spaces or use the
  //the ONVIF namespace "tns1:"

    /* TODO 3: Add the SendState topic keys and nice names. */

    //EVENT DATA INSTANCE
    // A bool data instance called "active" will hold the event state 0 or 1  
    /* TODO 4: Add and mark the active state property as data. */
    
    //Note that the 3:rd parameter defines if he event is stateful or stateless.  1 = stateless, 0 = stateful
    /* TODO 5: Declare the event as stateful. */
    
    // The key/value set is no longer needed
    ax_event_key_value_set_free( key_value_set );
    return declaration;
}

int main(void) {
    GMainLoop* main_loop = NULL;
    guint start_value = 0;

    openlog(SERVICE_ID, LOG_PID|LOG_CONS, LOG_USER);
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
