#include "vdo-error.h"
#include "vdo-map.h"
#include "vdo-stream.h"
#include "vdo-types.h"

#include <glib.h>
#include <glib/gstdio.h>
// Needed for g_autoptr
#include <glib-object.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <syslog.h>

#include "panic.h"

#include <poll.h>
#include <unistd.h>

 

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

int main(int argc, char** argv) {
    (void)argc;

    /* TODO 1: Initialize logging and local VDO variables. */
    /* TODO 2: Create the H.264 blocking stream. */
    /* TODO 3: Start the stream and log the stream info returned by VDO. */
    /* TODO 4: Fetch 10 buffers with the blocking get-buffer call. */
    /* TODO 5: Read frame metadata and return each buffer to VDO. */
    /* TODO 6: Release VDO objects and exit. */

    return 0;
}
