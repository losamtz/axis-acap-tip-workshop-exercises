#include <fcgiapp.h>
#include <jansson.h>
#include <axsdk/axparameter.h>
#include <glib-unix.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/stat.h>


#define APP_NAME "web_parameter"     // Must match your package/app name
#define FCGI_SOCKET_NAME "FCGI_SOCKET_NAME"

// ─── Helpers ──────────────────────────────────────────────────────────────────

__attribute__((noreturn)) __attribute__((format(printf, 1, 2))) static void
panic(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsyslog(LOG_ERR, fmt, args);
    va_end(args);
    exit(1);
}

static char* read_request_body(FCGX_Request* req) {

    const char* content_length = FCGX_GetParam("CONTENT_LENGTH", req->envp);
    if (!content_length) 
        return NULL;

    long len = strtol(content_length, NULL, 10);

    if (len <= 0 || len > 1<<20) 
        return NULL; // guard (<=1MB)

    char* buf = (char*)malloc(len + 1);
    if (!buf) 
        return NULL;

    int nread = FCGX_GetStr(buf, len, req->in);

    buf[(nread < len) ? nread : len] = '\0';

    return buf;
}

static void write_json_header(FCGX_Request* req, int status) {
    FCGX_FPrintF(req->out,
                 "Status: %d\r\n"
                 "Content-Type: application/json\r\n"
                 "Cache-Control: no-store\r\n"
                 "\r\n", status);
}

static void write_json_object(FCGX_Request* req, json_t* obj) {
    char* s = json_dumps(obj, JSON_COMPACT);
    FCGX_FPrintF(req->out, "%s", s ? s : "{}");
    free(s);
}

// ─── AXParameter helpers ──────────────────────────────────────────────────────

static AXParameter* init_axparameter(void) {

    GError* error = NULL;
    AXParameter* handle = ax_parameter_new(APP_NAME, &error);

    if (!handle) 
        panic("ax_parameter_new: %s", error->message);

    syslog(LOG_INFO, "AXParameter handler has been instanciated");

    return handle;
}

static bool param_get_string(AXParameter* handle, const char* name, char** out) {
    GError* error = NULL;
    char* value = NULL;

    if (!ax_parameter_get(handle, name, &value, &error)) {
        if (error) { syslog(LOG_WARNING, "get(%s): %s", name, error->message); g_error_free(error); }
        return false;
    }
    *out = value;             // caller must g_free
    return true;
}

static bool param_set_string(AXParameter* handle, const char* name, const char* value) {
    GError* error = NULL;

    if (!ax_parameter_set(handle, name, value, FALSE, &error)) { // FALSE = don't invoke callbacks
        if (error) { syslog(LOG_WARNING, "set(%s): %s", name, error->message); g_error_free(error); }
        return false;
    }
    return true;
}

// ─── Endpoint handlers ────────────────────────────────────────────────────────

static void handle_info(FCGX_Request* req, AXParameter* handle) {
    // GET current values
    char* addr = NULL;
    char* port = NULL;
    (void)param_get_string(handle, "MulticastAddress", &addr);
    (void)param_get_string(handle, "MulticastPort", &port);

    json_t* root = json_object();
    json_object_set_new(root, "ok", json_true());
    json_object_set_new(root, "MulticastAddress", addr ? json_string(addr) : json_null());
    json_object_set_new(root, "MulticastPort",   port ? json_string(port) : json_null());

    write_json_header(req, 200);
    write_json_object(req, root);
    json_decref(root);

    if (addr) g_free(addr);
    if (port) g_free(port);
}

static void handle_param(FCGX_Request* req, AXParameter* h) {
    // Expect POST with JSON body: { "MulticastAddress": "...", "MulticastPort": "..." }
    char* body = read_request_body(req);
    if (!body) {
        write_json_header(req, 400);
        FCGX_FPrintF(req->out, "{\"ok\":false,\"error\":\"Missing or empty body\"}");
        return;
    }

    json_error_t jerr;
    json_t* root = json_loads(body, 0, &jerr);
    free(body);

    if (!root || !json_is_object(root)) {
        write_json_header(req, 400);
        FCGX_FPrintF(req->out, "{\"ok\":false,\"error\":\"Invalid JSON\"}");
        if (root) json_decref(root);
        return;
    }

    const json_t* jAddr = json_object_get(root, "MulticastAddress");
    const json_t* jPort = json_object_get(root, "MulticastPort");

    bool changed = false;
    json_t* result = json_object();
    json_object_set_new(result, "ok", json_true());

    if (jAddr && json_is_string(jAddr)) {
        const char* s = json_string_value(jAddr);
        if (param_set_string(h, "MulticastAddress", s)) {
            json_object_set_new(result, "MulticastAddress", json_string(s));
            changed = true;
        } else {
            json_object_set_new(result, "MulticastAddressError", json_string("Failed to set"));
        }
    }
    if (jPort && json_is_string(jPort)) {
        const char* s = json_string_value(jPort);
        if (param_set_string(h, "MulticastPort", s)) {
            json_object_set_new(result, "MulticastPort", json_string(s));
            changed = true;
        } else {
            json_object_set_new(result, "MulticastPortError", json_string("Failed to set"));
        }
    }

    json_object_set_new(result, "changed", changed ? json_true() : json_false());
    write_json_header(req, 200);
    write_json_object(req, result);
    json_decref(result);
    json_decref(root);
}

// ─── Router ──────────────────────────────────────────────────────────────────

static void route_request(FCGX_Request* req, AXParameter* h) {
    const char* script = FCGX_GetParam("SCRIPT_NAME", req->envp);
    const char* method = FCGX_GetParam("REQUEST_METHOD", req->envp);
    if (!script) script = "";
    if (!method) method = "GET";

    syslog(LOG_INFO, "FCGI %s %s", method, script);

    /* TODO 1: Dispatch GET info requests to handle_info(). */
    /* TODO 2: Dispatch POST parameter update requests to handle_param(). */
    /* TODO 3: Return a JSON 404 response for unknown routes. */
}

// ─── main ────────────────────────────────────────────────────────────────────

int main(void) {
    /* TODO 4: Initialize logging and read the FastCGI socket path. */
    /* TODO 5: Initialize FastCGI and AXParameter. */
    /* TODO 6: Open the FastCGI socket and initialize the request object. */
    /* TODO 7: Accept requests, route them to handlers, and finish each request. */
    /* TODO 8: Clean up AXParameter and logging. */

    return 0;
}
