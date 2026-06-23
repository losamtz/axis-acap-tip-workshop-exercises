// main.c
#define _GNU_SOURCE
#include <fcgiapp.h>
#include <jansson.h>
#include <axsdk/axparameter.h>
#include <pthread.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <glib-unix.h>

#define FCGI_SOCKET_NAME "FCGI_SOCKET_NAME"
#define APP_NAME "web_parameter_thread"   // ACAP app scope for AXParameter
#define ENDPOINT_SET "/local/web_parameter_thread/parameter-acap.cgi"
#define ENDPOINT_GET "/local/web_parameter_thread/information-acap.cgi"

static AXParameter* handle = NULL;
static pthread_mutex_t handle_mtx = PTHREAD_MUTEX_INITIALIZER;

/* ---------- logging panic ---------- */
__attribute__((noreturn)) __attribute__((format(printf,1,2)))
static void panic(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsyslog(LOG_ERR, fmt, ap);
    va_end(ap);
    closelog();
    exit(EXIT_FAILURE);
}

/* ---------- AXParameter helpers (thread-safe) ---------- */
static gboolean add_if_missing(const char* name, const char* def, const char* meta) {
    GError* err = NULL;
    if (!ax_parameter_add(handle, name, def, meta, &err)) {
        if (err && err->code == AX_PARAMETER_PARAM_ADDED_ERROR) {
            g_error_free(err);
            return TRUE; // already exists
        } else {
            if (err) {
                syslog(LOG_ERR, "ax_parameter_add(%s) failed: %s", name, err->message);
                g_error_free(err);
            }
            return FALSE;
        }
    }
    return TRUE;
}

static char* get_param_dup(const char* name) {
    GError* err = NULL;
    char* out = NULL;
    if (!ax_parameter_get(handle, name, &out, &err)) {
        if (err) {
            syslog(LOG_WARNING, "ax_parameter_get(%s) failed: %s", name, err->message);
            g_error_free(err);
        }
        return NULL;
    }
    // ax_parameter_get allocs; return as is (caller frees)
    return out;
}

static gboolean set_param(const char* name, const char* value, gboolean run_cb) {
    GError* err = NULL;
    gboolean ok = ax_parameter_set(handle, name, value, run_cb, &err);
    if (!ok && err) {
        syslog(LOG_ERR, "ax_parameter_set(%s=%s) failed: %s", name, value, err->message);
        g_error_free(err);
    }
    return ok;
}

/* ---------- FastCGI helpers ---------- */
static void send_json(FCGX_Request* req, int status_code, json_t* obj) {
    char* body = json_dumps(obj, JSON_COMPACT);
    FCGX_FPrintF(req->out,
                 "Status: %d\r\n"
                 "Content-Type: application/json\r\n"
                 "Cache-Control: no-store\r\n"
                 "Access-Control-Allow-Origin: *\r\n"
                 "Access-Control-Allow-Headers: content-type\r\n"
                 "\r\n"
                 "%s",
                 status_code, body ? body : "{}");
    free(body);
}

static json_t* read_json_body(FCGX_Request* req) {
    const char* cl_str = FCGX_GetParam("CONTENT_LENGTH", req->envp);
    long len = cl_str ? strtol(cl_str, NULL, 10) : 0;
    if (len <= 0) return NULL;

    char* buf = (char*)malloc((size_t)len + 1);
    if (!buf) return NULL;

    int read_total = 0;
    while (read_total < len) {
        int n = FCGX_GetStr(buf + read_total, len - read_total, req->in);
        if (n <= 0) break;
        read_total += n;
    }
    buf[read_total] = '\0';

    json_error_t err;
    json_t* parsed = json_loads(buf, 0, &err);
    if (!parsed) {
        syslog(LOG_ERR, "JSON parse error: %s at line %d", err.text, err.line);
    }
    free(buf);
    return parsed;
}

/* ---------- Routing handlers ---------- */
static void handle_info(FCGX_Request* req) {
    json_t* out = json_object();

    pthread_mutex_lock(&handle_mtx);
    char* addr = get_param_dup("IpAddress");
    char* port = get_param_dup("Port");
    pthread_mutex_unlock(&handle_mtx);

    if (addr) {
        json_object_set_new(out, "IpAddress", json_string(addr));
        free(addr);
    } else {
        json_object_set_new(out, "IpAddress", json_null());
    }

    if (port) {
        json_object_set_new(out, "Port", json_string(port));
        free(port);
    } else {
        json_object_set_new(out, "Port", json_null());
    }

    json_object_set_new(out, "ok", json_true());
    send_json(req, 200, out);
    json_decref(out);
}

static void handle_param(FCGX_Request* req) {
    json_t* body = read_json_body(req);
    if (!body || !json_is_object(body)) {
        json_t* err = json_pack("{s:s}", "error", "Invalid JSON body");
        send_json(req, 400, err);
        json_decref(err);
        if (body) json_decref(body);
        return;
    }

    const char* addr = NULL;
    const char* port = NULL;

    json_t* jaddr = json_object_get(body, "IpAddress");
    if (jaddr && json_is_string(jaddr)) addr = json_string_value(jaddr);

    json_t* jport = json_object_get(body, "Port");
    if (jport && json_is_string(jport)) port = json_string_value(jport);
    if (jport && json_is_integer(jport)) {
        static char tmp[32];
        snprintf(tmp, sizeof(tmp), "%" JSON_INTEGER_FORMAT, json_integer_value(jport));
        port = tmp;
    }

    if (!addr && !port) {
        json_t* err = json_pack("{s:s}", "error", "No fields to update");
        send_json(req, 400, err);
        json_decref(err);
        json_decref(body);
        return;
    }

    pthread_mutex_lock(&handle_mtx);
    gboolean ok = TRUE;
    if (addr) ok = ok && set_param("IpAddress", addr, FALSE);
    if (port) ok = ok && set_param("Port", port, FALSE);
    pthread_mutex_unlock(&handle_mtx);

    json_t* out = json_object();
    json_object_set_new(out, "ok", json_boolean(ok));
    if (!ok) json_object_set_new(out, "error", json_string("Failed to set one or more parameters"));
    send_json(req, ok ? 200 : 500, out);
    json_decref(out);
    json_decref(body);
}

/* ---------- Simple router ---------- */
static void handle_request(FCGX_Request* req) {
    const char* script = FCGX_GetParam("SCRIPT_NAME", req->envp);
    if (!script) script = "";

    if (strcmp(script, ENDPOINT_GET) == 0) {
        handle_info(req);
        return;
    }
    if (strcmp(script, ENDPOINT_SET) == 0) {
        handle_param(req);
        return;
    }

    // Unknown endpoint
    json_t* err = json_pack("{s:s,s:s}", "error", "Unknown endpoint", "script", script);
    send_json(req, 404, err);
    json_decref(err);
}

/* ---------- main ---------- */

int main(void) {
    /* TODO 1: Review the README steps for manifest and Makefile changes. */
    /* TODO 2: Paste the setup snippet into this main function. */
    /* TODO 3: Paste the runtime/API workflow snippets in order. */
    /* TODO 4: Paste the cleanup snippet at the end. */

    return 0;
}
