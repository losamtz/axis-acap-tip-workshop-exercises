/*
 * Minimal ACAP application for explaining package structure.
 */

#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

static volatile sig_atomic_t running = 1;

static void signal_handler(int signal_number) {
    (void)signal_number;
    running = 0;
}

int main(void) {
    /* TODO 1: Review the README steps for manifest and Makefile changes. */
    /* TODO 2: Paste the setup snippet into this main function. */
    /* TODO 3: Paste the runtime/API workflow snippets in order. */
    /* TODO 4: Paste the cleanup snippet at the end. */

    return 0;
}
