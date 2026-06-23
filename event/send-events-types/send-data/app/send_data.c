#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <stdlib.h> // For rand() and srand()
#include <axsdk/axevent.h>
#include <glib-object.h>
#include <glib.h>

#define LOG(fmt, args...)    { syslog(LOG_INFO, fmt, ## args); printf(fmt, ## args); }
#define LOG_ERROR(fmt, args...)    { syslog(LOG_CRIT, fmt, ## args); printf(fmt, ## args); }

#define SERVICE_ID   "send-data"

#define TOPIC0_TAG  "CameraApplicationPlatform"
#define TOPIC0_NAME "ACAP"
#define TOPIC1_TAG  "SendData"
#define TOPIC1_NAME "Send Data"
#define EVENT_TAG   "SendDataEvent"
#define EVENT_NAME  "Send Data Event"

#define TOTAL_MEMORY 500 // Simulate total memory (e.g. 500MB)

typedef struct {
  AXEventHandler *event_handler;
  guint event_id;
  guint timer;
  gdouble temperature;
  gdouble load;
  guint free_memory;
  guint used_memory;
} AppData;

static AppData* app_data = NULL;

static void generate_random_data(AppData *app_data) {

  // Simulate temperature between 20.0 and 80.0 degrees Celsius
  app_data->temperature = 20.0 + (rand() % 6000) / 100.0;  // 20.0 to 80.0

  // Simulate system load as a float between 0.0 and 4.0 (e.g. CPU load avg)
  app_data->load = (rand() % 400) / 100.0;

  

  // Simulate used memory between 0 and total
  app_data->used_memory = rand() % TOTAL_MEMORY;
  
  // Free memory is the rest
  app_data->free_memory = TOTAL_MEMORY - app_data->used_memory;

  LOG("used memory: %u MB, free memory: %u MB\n", app_data->used_memory, app_data->free_memory);
}

static gboolean send_data(AppData *send_data) {

  AXEventKeyValueSet *key_value_set = NULL;
  AXEvent *event = NULL;

  generate_random_data(send_data);
  key_value_set = ax_event_key_value_set_new();

  /* TODO 1: Add the runtime data values to the event key/value set. */

  // Create the event
  event = ax_event_new2(key_value_set, NULL);

  // The key/value set is no longer needed
  ax_event_key_value_set_free(key_value_set);

  /* TODO 2: Send the event with the declared event id. */

  ax_event_free(event);

  // Returning TRUE keeps the timer going
  return TRUE;
}

static void declaration_complete(guint declaration, gdouble *value) {

    LOG("Declaration complete for: %d\n", declaration);
    
    app_data->temperature = *value;
    app_data->load = 0.0;
    app_data->used_memory = 0;
    app_data->free_memory = 0;

    // timer to be called every 3th second
    app_data->timer = g_timeout_add_seconds(3, (GSourceFunc)send_data, app_data);

}

static guint setup_declaration(AXEventHandler *event_handler) {

      AXEventKeyValueSet *key_value_set = NULL;
      guint declaration                 = 0;
      
      gdouble start_value               = 0.0;

      gdouble temperature               = 0.5;
      gdouble load                      = 0.0;
      guint used_memory                 = 0;
      guint free_memory                 = 0;
      //GError *error                     = NULL;

      
      key_value_set = ax_event_key_value_set_new();

      // OMITING ERROR HANDLING!

      /* TODO 3: Declare the SendData topic and data schema. */
      
      //Note that the 3:rd parameter defines if the event is stateful or stateless.  1 = stateless, 0 = stateful
      /* TODO 4: Declare the stateless event. */

      ax_event_key_value_set_free(key_value_set);

      return declaration;
}

int main(void) {
    GMainLoop* main_loop = NULL;

    openlog(SERVICE_ID, LOG_PID|LOG_CONS, LOG_USER);
    main_loop = g_main_loop_new(NULL, FALSE);

    app_data = calloc(1, sizeof(AppData));
    app_data->event_handler = ax_event_handler_new();
    app_data->event_id = setup_declaration(app_data->event_handler);

    g_main_loop_run(main_loop);

    ax_event_handler_undeclare(app_data->event_handler, app_data->event_id, NULL);
    ax_event_handler_free(app_data->event_handler);
    free(app_data);
    g_main_loop_unref(main_loop);
    closelog();

    return 0;
}
