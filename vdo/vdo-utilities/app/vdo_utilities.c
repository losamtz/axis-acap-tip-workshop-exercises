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
    /* TODO 1: Initialize logging and local VDO variables. */
    /* TODO 2: Log available channels, filtered input channels, and supported resolutions. */
    /* TODO 3: Create an RGB stream only to inspect stream metadata. */
    /* TODO 4: Log stream rotation and clean up the stream. */

    return 0;
}
