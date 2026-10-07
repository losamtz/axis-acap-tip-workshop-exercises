/* Parameter ACAP upgrade app with AXParameter and JSON-backed settings. */
#include <axsdk/axparameter.h>
#include <errno.h>
#include <fcgiapp.h>
#include <glib-unix.h>
#include <glib.h>
#include <jansson.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

#include "panic.h"

static AXParameter* axparameter = NULL;
static GMainLoop* main_loop = NULL;
// Guards axparameter calls from the FastCGI thread and the main loop.
static GMutex param_mutex;
// Guards owned_name and events, shared with the FastCGI thread.
static GMutex state_mutex;
static gchar* owned_name = NULL;
static GQueue events = G_QUEUE_INIT;
// Main-loop thread only.
static gchar* last_seen_name = NULL;
static guint revert_source = 0;

#define MAX_EVENTS 20
#define REVERT_DELAY_SECONDS 10
#define FCGI_SOCKET_NAME "FCGI_SOCKET_NAME"
#define MAX_REQUEST_BODY 4096
#define LOCALDATA_DIR "/usr/local/packages/parameter_ACAP_upgrade/localdata"
#define CONFIG_PATH LOCALDATA_DIR "/configuration.json"
#define CONFIG_TEMP_PATH LOCALDATA_DIR "/configuration.json.tmp"
#define DEFAULT_BACKGROUND_COLOUR "#f3f5f2"

static void add_event(const char* format, ...) G_GNUC_PRINTF(1, 2);

static void add_event(const char* format, ...) {
    va_list args;
    va_start(args, format);
    gchar* message = g_strdup_vprintf(format, args);
    va_end(args);

    GDateTime* now = g_date_time_new_now_local();
    gchar* time_text = g_date_time_format(now, "%H:%M:%S");
    g_date_time_unref(now);

    syslog(LOG_INFO, "%s", message);

    g_mutex_lock(&state_mutex);
    g_queue_push_tail(&events, g_strdup_printf("%s  %s", time_text, message));
    while (g_queue_get_length(&events) > MAX_EVENTS) {
        g_free(g_queue_pop_head(&events));
    }
    g_mutex_unlock(&state_mutex);

    g_free(time_text);
    g_free(message);
}

static gchar* get_acap_name(void) {
    gchar* value = NULL;
    GError* error = NULL;

    g_mutex_lock(&param_mutex);
    gboolean success = axparameter && ax_parameter_get(axparameter, "ACAPName", &value, &error);
    g_mutex_unlock(&param_mutex);

    if (!success) {
        syslog(LOG_ERR, "Failed to read ACAPName: %s", error ? error->message : "application is stopping");
        g_clear_error(&error);
        return NULL;
    }
    return value;
}

static gboolean set_acap_name(const char* value) {
    GError* error = NULL;

    g_mutex_lock(&param_mutex);
    gboolean success = axparameter && ax_parameter_set(axparameter, "ACAPName", value, TRUE, &error);
    g_mutex_unlock(&param_mutex);

    if (!success) {
        syslog(LOG_ERR, "Failed to set ACAPName: %s", error ? error->message : "application is stopping");
        g_clear_error(&error);
    }
    return success;
}

static gboolean revert_acap_name(gpointer user_data) {
    (void)user_data;
    revert_source = 0;

    g_mutex_lock(&state_mutex);
    gchar* name = g_strdup(owned_name);
    g_mutex_unlock(&state_mutex);

    if (set_acap_name(name)) {
        add_event("ACAPName restored to '%s'. I am the master of my parameters here!", name);
    }
    g_free(name);
    return G_SOURCE_REMOVE;
}

// Runs on the main loop; ax_parameter_* must not be called from here, so the revert is deferred.
static void acap_name_callback(const gchar* name, const gchar* value, gpointer user_data) {
    (void)name;
    (void)user_data;
    if (!value) {
        return;
    }

    g_mutex_lock(&state_mutex);
    gboolean external = g_strcmp0(value, owned_name) != 0;
    g_mutex_unlock(&state_mutex);

    if (external) {
        add_event("ACAPName changed outside the ACAP from '%s' to '%s'. Restoring it in %d seconds.",
                  last_seen_name,
                  value,
                  REVERT_DELAY_SECONDS);
        if (revert_source) {
            g_source_remove(revert_source);
        }
        revert_source = g_timeout_add_seconds(REVERT_DELAY_SECONDS, revert_acap_name, NULL);
    }

    g_free(last_seen_name);
    last_seen_name = g_strdup(value);
}

static gboolean signal_handler(gpointer loop) {
    g_main_loop_quit((GMainLoop*)loop);
    syslog(LOG_INFO, "Application was stopped by SIGTERM or SIGINT.");
    return G_SOURCE_REMOVE;
}

static void send_json_response(FCGX_Request* request, int status, const char* body) {
    FCGX_FPrintF(request->out,
                 "Status: %d\r\n"
                 "Content-Type: application/json; charset=utf-8\r\n"
                 "Cache-Control: no-store\r\n\r\n"
                 "%s",
                 status,
                 body);
}

static gboolean valid_background_colour(const char* colour) {
    if (!colour || strlen(colour) != 7 || colour[0] != '#') {
        return FALSE;
    }

    for (size_t index = 1; index < 7; index++) {
        if (!g_ascii_isxdigit(colour[index])) {
            return FALSE;
        }
    }
    return TRUE;
}

static gboolean save_background_colour(const char* colour) {
    json_t* configuration = json_object();
    if (!configuration || json_object_set_new(configuration,
                                              "backgroundColour",
                                              json_string(colour)) != 0) {
        if (configuration) {
            json_decref(configuration);
        }
        return FALSE;
    }

    int result = json_dump_file(configuration, CONFIG_TEMP_PATH, JSON_INDENT(2));
    json_decref(configuration);
    if (result != 0) {
        unlink(CONFIG_TEMP_PATH);
        return FALSE;
    }

    if (rename(CONFIG_TEMP_PATH, CONFIG_PATH) != 0) {
        unlink(CONFIG_TEMP_PATH);
        return FALSE;
    }
    return TRUE;
}

static gboolean load_background_colour(char colour[8]) {
    json_error_t error;
    errno = 0;
    json_t* configuration = json_load_file(CONFIG_PATH, 0, &error);
    if (!configuration) {
        if (errno == ENOENT) {
            g_strlcpy(colour, DEFAULT_BACKGROUND_COLOUR, 8);
            return save_background_colour(colour);
        }

        syslog(LOG_WARNING, "Unable to read JSON configuration: %s", error.text);
        g_strlcpy(colour, DEFAULT_BACKGROUND_COLOUR, 8);
        return TRUE;
    }

    json_t* value = json_object_get(configuration, "backgroundColour");
    const char* stored_colour = json_is_string(value) ? json_string_value(value) : NULL;
    if (valid_background_colour(stored_colour)) {
        g_strlcpy(colour, stored_colour, 8);
    } else {
        g_strlcpy(colour, DEFAULT_BACKGROUND_COLOUR, 8);
    }

    json_decref(configuration);
    return TRUE;
}

static void render_settings_page(FCGX_Request* request) {
    gchar* acap_name = get_acap_name();
    char background_colour[8];

    if (!acap_name) {
        send_json_response(request, 500, "{\"ok\":false,\"error\":\"Unable to read settings\"}");
        return;
    }

    if (!load_background_colour(background_colour)) {
        syslog(LOG_ERR, "Failed to initialize JSON configuration: %s", strerror(errno));
        g_free(acap_name);
        send_json_response(request, 500, "{\"ok\":false,\"error\":\"Unable to load settings\"}");
        return;
    }

    gchar* escaped_name = g_markup_escape_text(acap_name, -1);

    FCGX_FPrintF(request->out,
                 "Content-Type: text/html; charset=utf-8\r\n"
                 "Cache-Control: no-store\r\n\r\n"
                 "<!doctype html>"
                 "<html lang=\"en\"><head><meta charset=\"utf-8\">"
                 "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                 "<title>%s</title>"
                 "<style>"
                 ":root{font-family:system-ui,sans-serif;color:#202827}"
                 "*{box-sizing:border-box}body{margin:0;min-height:100vh}"
                 "body{background-color:%s;color:#202827}header{padding:2rem max(1.25rem,calc((100%% - 42rem)/2));"
                 "background:#243b37;color:#fff}header p{margin:0 0 .5rem;color:#b8d2ca;font-size:.8rem}"
                 "h1{font-size:1.65rem;margin:0;overflow-wrap:anywhere}"
                 "main{max-width:42rem;margin:2rem auto;padding:0 1.25rem}"
                 "form{background:#fff;border:1px solid #d8dfda;border-radius:6px;padding:1.5rem}"
                 "h2{font-size:1.1rem;margin:0 0 1.25rem}label{display:block;font-weight:650;margin:1rem 0 .35rem}"
                 "input[type=text]{width:100%%;max-width:28rem;padding:.65rem .75rem;border:1px solid #aebbb4;"
                 "border-radius:4px;font:inherit}input[type=color]{display:block;width:4rem;height:2.75rem;"
                 "padding:.2rem;border:1px solid #aebbb4;border-radius:4px;background:#fff}"
                 "input:focus{outline:2px solid #007f70;outline-offset:1px}"
                 ".hint{color:#52615a;font-size:.86rem;margin:.35rem 0 0}"
                 "button{margin-top:1.4rem;padding:.65rem 1rem;border:0;border-radius:4px;"
                 "background:#bd352b;color:#fff;font:inherit;font-weight:650;cursor:pointer}"
                 "button:disabled{opacity:.6;cursor:wait}#status{min-height:1.5rem;margin:1rem 0 0;font-size:.9rem}"
                 "#status[data-state=\"error\"]{color:#a1221b}#status[data-state=\"success\"]{color:#006b55}"
                 ".log{margin-top:1.5rem;background:#fff;border:1px solid #d8dfda;border-radius:6px;padding:1.5rem}"
                 "#events{margin:0;padding:0;list-style:none;font-family:ui-monospace,monospace;font-size:.85rem}"
                 "#events li{padding:.45rem 0;border-top:1px solid #e6ebe8}#events li:first-child{border-top:0}"
                 "</style></head><body style=\"background-color:%s\">"
                 "<header><p>PARAMETER ACAP UPGRADE</p><h1 id=\"acap-name\">%s</h1></header>"
                 "<main><form id=\"settings\"><h2>Application settings</h2>"
                 "<label for=\"acap-name-input\">ACAP name</label>"
                 "<input id=\"acap-name-input\" name=\"ACAPName\" type=\"text\" value=\"%s\" required maxlength=\"64\">"
                 "<p class=\"hint\">Stored as an application parameter and shown above.</p>"
                 "<label for=\"background-colour\">Background colour</label>"
                 "<input id=\"background-colour\" name=\"backgroundColour\" type=\"color\" value=\"%s\">"
                 "<p class=\"hint\">Stored in the app's JSON configuration file.</p>"
                 "<button id=\"save\" type=\"submit\">Save settings</button>"
                 "<p id=\"status\" role=\"status\" aria-live=\"polite\"></p></form>"
                 "<section class=\"log\"><h2>Parameter log</h2><ul id=\"events\" aria-live=\"polite\"></ul></section></main>"
                 "<script>"
                 "const form=document.querySelector('#settings');"
                 "const statusText=document.querySelector('#status');"
                 "form.addEventListener('submit',async event=>{event.preventDefault();"
                 "const button=document.querySelector('#save');button.disabled=true;"
                 "statusText.dataset.state='';statusText.textContent='Saving…';"
                 "try{const response=await fetch(location.pathname,{method:'POST',"
                 "headers:{'Content-Type':'application/json'},"
                 "body:JSON.stringify(Object.fromEntries(new FormData(form)))});"
                 "const result=await response.json();if(!response.ok||!result.ok)"
                 "throw new Error(result.error||'Unable to save settings');"
                 "const name=form.elements.ACAPName.value.trim();"
                 "document.querySelector('#acap-name').textContent=name;document.title=name;"
                 "document.body.style.backgroundColor=form.elements.backgroundColour.value;"
                 "statusText.dataset.state='success';statusText.textContent='Settings saved.';"
                 "}catch(error){statusText.dataset.state='error';statusText.textContent=error.message;"
                 "}finally{button.disabled=false;}});"
                 "const eventList=document.querySelector('#events');"
                 "async function refreshEvents(){try{"
                 "const response=await fetch(location.pathname+'?events',{cache:'no-store'});"
                 "const result=await response.json();if(!response.ok||!result.ok)return;"
                 "if(typeof result.acapName==='string'){"
                 "document.querySelector('#acap-name').textContent=result.acapName;document.title=result.acapName;}"
                 "const items=result.events.slice().reverse().map(text=>{"
                 "const item=document.createElement('li');item.textContent=text;return item;});"
                 "if(!items.length){const item=document.createElement('li');"
                 "item.textContent='No external changes yet.';items.push(item);}"
                 "eventList.replaceChildren(...items);}catch(error){}}"
                 "refreshEvents();setInterval(refreshEvents,2000);"
                 "</script></body></html>",
                 escaped_name,
                 background_colour,
                 background_colour,
                 escaped_name,
                 escaped_name,
                 background_colour);

    g_free(escaped_name);
    g_free(acap_name);
}

static gboolean valid_acap_name(const char* name) {
    return name && *name && strlen(name) <= 64 && g_utf8_validate(name, -1, NULL);
}

static json_t* read_json_body(FCGX_Request* request) {
    const char* length_text = FCGX_GetParam("CONTENT_LENGTH", request->envp);
    char* end = NULL;
    long length = length_text ? strtol(length_text, &end, 10) : 0;

    if (!length_text || !end || *end != '\0' || length <= 0 || length > MAX_REQUEST_BODY) {
        return NULL;
    }

    char* buffer = malloc((size_t)length + 1);
    if (!buffer) {
        return NULL;
    }

    int bytes_read = 0;
    while (bytes_read < length) {
        int count = FCGX_GetStr(buffer + bytes_read, (int)(length - bytes_read), request->in);
        if (count <= 0) {
            free(buffer);
            return NULL;
        }
        bytes_read += count;
    }
    buffer[bytes_read] = '\0';

    json_error_t json_error;
    json_t* body = json_loadb(buffer, (size_t)bytes_read, 0, &json_error);
    free(buffer);
    return body;
}

static void update_settings(FCGX_Request* request) {
    json_t* body = read_json_body(request);
    json_t* name_value;
    json_t* colour_value;
    const char* acap_name;
    const char* background_colour;

    if (!body || !json_is_object(body)) {
        if (body) {
            json_decref(body);
        }
        send_json_response(request, 400, "{\"ok\":false,\"error\":\"Invalid request body\"}");
        return;
    }

    name_value = json_object_get(body, "ACAPName");
    colour_value = json_object_get(body, "backgroundColour");
    if (!json_is_string(name_value) || !json_is_string(colour_value)) {
        json_decref(body);
        send_json_response(request, 400, "{\"ok\":false,\"error\":\"ACAP name and background colour are required\"}");
        return;
    }

    acap_name = json_string_value(name_value);
    background_colour = json_string_value(colour_value);
    if (json_string_length(name_value) != strlen(acap_name) || !valid_acap_name(acap_name) ||
        !valid_background_colour(background_colour)) {
        json_decref(body);
        send_json_response(request, 400, "{\"ok\":false,\"error\":\"Invalid ACAP name or background colour\"}");
        return;
    }

    // Claim the new value first so the callback recognises this change as our own.
    g_mutex_lock(&state_mutex);
    gchar* previous_name = owned_name;
    owned_name = g_strdup(acap_name);
    g_mutex_unlock(&state_mutex);

    if (!set_acap_name(acap_name)) {
        g_mutex_lock(&state_mutex);
        g_free(owned_name);
        owned_name = previous_name;
        g_mutex_unlock(&state_mutex);
        json_decref(body);
        send_json_response(request, 500, "{\"ok\":false,\"error\":\"Unable to save ACAP name\"}");
        return;
    }
    g_free(previous_name);

    if (!save_background_colour(background_colour)) {
        syslog(LOG_ERR, "Failed to save background colour: %s", strerror(errno));
        json_decref(body);
        send_json_response(request, 500, "{\"ok\":false,\"error\":\"Unable to save background colour\"}");
        return;
    }

    json_decref(body);
    send_json_response(request, 200, "{\"ok\":true}");
}

static void send_events(FCGX_Request* request) {
    gchar* acap_name = get_acap_name();
    json_t* response = json_object();
    json_t* list = json_array();

    g_mutex_lock(&state_mutex);
    for (GList* item = events.head; item; item = item->next) {
        json_array_append_new(list, json_string(item->data));
    }
    g_mutex_unlock(&state_mutex);

    json_object_set_new(response, "ok", json_true());
    json_object_set_new(response, "acapName", acap_name ? json_string(acap_name) : json_null());
    json_object_set_new(response, "events", list);
    g_free(acap_name);

    char* body = json_dumps(response, JSON_COMPACT);
    json_decref(response);
    if (body) {
        send_json_response(request, 200, body);
        free(body);
    } else {
        send_json_response(request, 500, "{\"ok\":false,\"error\":\"Unable to read events\"}");
    }
}

static void handle_request(FCGX_Request* request) {
    const char* method = FCGX_GetParam("REQUEST_METHOD", request->envp);
    const char* query = FCGX_GetParam("QUERY_STRING", request->envp);

    if (method && strcmp(method, "GET") == 0 && query && strcmp(query, "events") == 0) {
        send_events(request);
    } else if (method && strcmp(method, "GET") == 0) {
        render_settings_page(request);
    } else if (method && strcmp(method, "POST") == 0) {
        update_settings(request);
    } else {
        send_json_response(request, 405, "{\"ok\":false,\"error\":\"Method not allowed\"}");
    }
}

static gpointer fastcgi_thread(gpointer user_data) {
    (void)user_data;
    const char* socket_path = g_getenv(FCGI_SOCKET_NAME);
    if (!socket_path) {
        panic("Missing %s environment variable", FCGI_SOCKET_NAME);
    }

    if (FCGX_Init() != 0) {
        panic("FCGX_Init failed");
    }

    int socket_fd = FCGX_OpenSocket(socket_path, 5);
    if (socket_fd < 0) {
        panic("FCGX_OpenSocket failed for %s", socket_path);
    }
    if (chmod(socket_path, S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
        panic("Failed to set permissions on FastCGI socket %s", socket_path);
    }

    FCGX_Request request;
    if (FCGX_InitRequest(&request, socket_fd, 0) != 0) {
        panic("FCGX_InitRequest failed");
    }

    syslog(LOG_INFO, "FastCGI settings endpoint listening on %s", socket_path);
    while (FCGX_Accept_r(&request) == 0) {
        handle_request(&request);
        FCGX_Finish_r(&request);
    }

    syslog(LOG_ERR, "FastCGI request loop stopped");
    return NULL;
}

int main(int argc, char** argv) {
    (void)argc;
    GError* error = NULL;
    char* app_name = basename(argv[0]);

    openlog(app_name, LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Starting %s", app_name);

    axparameter = ax_parameter_new(app_name, &error);
    if (!axparameter) {
        panic("ax_parameter_new failed: %s", error ? error->message : "unknown error");
    }

    if (!ax_parameter_get(axparameter, "ACAPName", &owned_name, &error)) {
        panic("Failed to read ACAPName: %s", error->message);
    }
    last_seen_name = g_strdup(owned_name);

    if (!ax_parameter_register_callback(axparameter, "ACAPName", acap_name_callback, NULL, &error)) {
        panic("register ACAPName failed: %s", error->message);
    }

    if (g_mkdir_with_parents(LOCALDATA_DIR, 0750) != 0) {
        panic("Failed to create app data directory %s: %s", LOCALDATA_DIR, strerror(errno));
    }

    char background_colour[8];
    if (!load_background_colour(background_colour)) {
        panic("Failed to initialize JSON configuration: %s", strerror(errno));
    }

    main_loop = g_main_loop_new(NULL, FALSE);
    g_unix_signal_add(SIGTERM, signal_handler, main_loop);
    g_unix_signal_add(SIGINT, signal_handler, main_loop);
    g_thread_unref(g_thread_new("fastcgi", fastcgi_thread, NULL));
    g_main_loop_run(main_loop);

    if (revert_source) {
        g_source_remove(revert_source);
    }
    g_mutex_lock(&param_mutex);
    ax_parameter_free(axparameter);
    axparameter = NULL;
    g_mutex_unlock(&param_mutex);
    g_main_loop_unref(main_loop);

    closelog();
    return EXIT_SUCCESS;
}
