/*
 * Exercise skeleton for overlay2_draw_rectangle.
 *
 * Open README.md in this example and paste the implementation snippets into
 * this file. The skeleton intentionally keeps error handling and setup minimal
 * so the exercise focuses on the ACAP API flow.
 */

#include <stdlib.h>
#include <syslog.h>

int main(void) {
    openlog("overlay2_draw_rectangle", LOG_PID, LOG_USER);

    syslog(LOG_INFO, "overlay2_draw_rectangle exercise skeleton started");

    /* TODO 1: Add the API-specific headers, constants, and global state from README.md. */
    /* TODO 2: Add helper functions, callbacks, and request handlers from README.md. */
    /* TODO 3: Replace this minimal main() with the setup and runtime flow from README.md. */
    /* TODO 4: Add cleanup/shutdown code at the end of the runtime flow. */

    syslog(LOG_INFO, "TODO: complete overlay2_draw_rectangle.c using the README implementation snippet");

    closelog();
    return EXIT_SUCCESS;
}
