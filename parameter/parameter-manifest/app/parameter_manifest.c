/*
 * Exercise skeleton for parameter_manifest.
 *
 * Open README.md in this example and paste the implementation snippets into
 * this file. The skeleton intentionally keeps error handling and setup minimal
 * so the exercise focuses on the ACAP API flow.
 */

#include <stdlib.h>
#include <syslog.h>

int main(void) {
    openlog("parameter_manifest", LOG_PID, LOG_USER);

    syslog(LOG_INFO, "parameter_manifest exercise skeleton started");

    /* TODO 1: Add the ParameterManifest entry to manifest.json. */
    /* TODO 2: Add axparameter to the Makefile PKGS line. */
    /* TODO 3: Replace the includes and APP_NAME constant from README.md. */
    /* TODO 4: Paste the signal handler from README.md. */
    /* TODO 5: Paste the parameter callback from README.md. */
    /* TODO 6: Replace this main() with the AXParameter setup, callback registration, loop, and cleanup. */

    syslog(LOG_INFO, "TODO: complete parameter_manifest.c using the README implementation snippet");

    closelog();
    return EXIT_SUCCESS;
}
