#include <gio/gio.h>
#include <stdio.h>
#include "panic.h"
#include "vapix-credentials.h"

static char* parse_credentials(GVariant* result) {
    char* credentials_string = NULL;
    char* id                 = NULL;
    char* password           = NULL;

    g_variant_get(result, "(&s)", &credentials_string);
    if (sscanf(credentials_string, "%m[^:]:%ms", &id, &password) != 2)
        panic("Error parsing credential string '%s'", credentials_string);
    char* credentials = g_strdup_printf("%s:%s", id, password);

    free(id);
    free(password);
    return credentials;
}

char* retrieve_vapix_credentials(const char* username) {
    /* TODO 2: Retrieve VAPIX service account credentials over D-Bus. */
}
