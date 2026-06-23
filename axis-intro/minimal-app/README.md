# Minimal App Exercise

This exercise is based on the corresponding complete example in `axis-acap-tip-workshop`.
The source file `app/axis_intro_minimal.c` has been reduced to a small TODO skeleton.

Your task is to rebuild the application flow by pasting the snippet below into `app/axis_intro_minimal.c`.
The snippet is intentionally kept in the README so you can read the sequence before editing the C file.

## What to do

1. Open `app/axis_intro_minimal.c`.
2. Replace the skeleton implementation with the code from **Implementation snippet** below.
3. Read through the code and identify the API setup, runtime loop, and cleanup flow.
4. Build the package with the commands in **Build**.


## Implementation snippet

Paste this into `app/axis_intro_minimal.c`:

```c
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
    openlog("axis_intro_minimal", LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Axis intro minimal app started");

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    while (running)
        sleep(1);

    syslog(LOG_INFO, "Axis intro minimal app stopped");
    closelog();
    return EXIT_SUCCESS;
}
```

## Build

From this example directory:

```sh
docker build --tag minimal-app --build-arg ARCH=aarch64 .
docker cp $(docker create minimal-app):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera from the Apps page or with your usual ACAP install flow.
If the application exposes HTTP endpoints or overlays, use the behavior described by the code comments and the parent module README to verify it.

## Reference

The complete version lives in the original `axis-acap-tip-workshop` repository under the same relative path:

`axis-intro/minimal-app`
