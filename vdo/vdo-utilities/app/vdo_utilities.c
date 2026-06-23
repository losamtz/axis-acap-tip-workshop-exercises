#include "vdo-error.h"
#include "vdo-map.h"
#include "vdo-stream.h"
#include "vdo-types.h"
#include <vdo-channel.h>
#include "panic.h"
#include "utilities.h"

#include <glib-unix.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <gmodule.h>

#define WITH 640
#define HEIGHT 360

static int handle_vdo_failed(GError* error) {
    // Maintenance/Installation in progress (e.g. Global-Rotation)
    if (vdo_error_is_expected(&error)) {
        syslog(LOG_INFO, "Expected vdo error %s", error->message);
        return EXIT_SUCCESS;
    } else {
        panic("Unexpected vdo error %s", error->message);
    }
    return EXIT_FAILURE;
}

int main(void) {
    /* TODO 1: Review the README steps for manifest and Makefile changes. */
    /* TODO 2: Paste the setup snippet into this main function. */
    /* TODO 3: Paste the runtime/API workflow snippets in order. */
    /* TODO 4: Paste the cleanup snippet at the end. */

    return 0;
}
