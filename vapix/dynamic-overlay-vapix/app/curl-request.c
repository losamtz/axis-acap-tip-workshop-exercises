#include <curl/curl.h>
#include <gio/gio.h>
#include "curl-request.h"
#include "panic.h"


static size_t append_to_gstring_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    size_t processed_bytes = size * nmemb;
    g_string_append_len((GString*)userdata, ptr, processed_bytes);
    return processed_bytes;
}

char* vapix_post(CURL* handle, const char* credentials, const char* endpoint, const char* request) {
    /* TODO 3: Configure and perform the VAPIX curl POST request. */
}
