# Vdo Utilities Exercise

This exercise is based on the corresponding complete example in `axis-acap-tip-workshop`.
The source file `app/vdo_utilities.c` has been reduced to a small TODO skeleton.

Your task is to rebuild the application flow by pasting the snippet below into `app/vdo_utilities.c`.
The snippet is intentionally kept in the README so you can read the sequence before editing the C file.

## What to do

1. Open `app/vdo_utilities.c`.
2. Replace the skeleton implementation with the code from **Implementation snippet** below.
3. Read through the code and identify the API setup, runtime loop, and cleanup flow.
4. Build the package with the commands in **Build**.


## Provided helper files

These helper files are left in place so the exercise can focus on the main application flow:

- `app/panic.c`
- `app/utilities.c`

## Implementation snippet

Paste this into `app/vdo_utilities.c`:

```c
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
    g_autoptr(GError) error = NULL;
    g_autoptr(VdoStream) vdo_stream = NULL;

    openlog(NULL, LOG_PID, LOG_USER);

    // 1- List channels & resolutions 
    get_video_channels();
    get_filtered_channels();
    get_stream_resolutions();
          
    // 3 - create stream without starting it to show how to retrieve stream information such as rotation. No need to start stream for that.
    vdo_stream = vdo_stream_rgb_new(NULL, 1u, (VdoResolution){ .width = WITH, .height = HEIGHT }, &error);

    if (!vdo_stream) {
        return handle_vdo_failed(error);
    }   

    // 4 - get rotation
    get_stream_rotation(vdo_stream);

    if (vdo_stream) {
        g_object_unref(vdo_stream);
    }
    

    syslog(LOG_INFO, "Exiting cleanly.");
    closelog();
    return EXIT_SUCCESS;
}
```

## Build

From this example directory:

```sh
docker build --tag vdo-utilities --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-utilities):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera from the Apps page or with your usual ACAP install flow.
If the application exposes HTTP endpoints or overlays, use the behavior described by the code comments and the parent module README to verify it.

## Reference

The complete version lives in the original `axis-acap-tip-workshop` repository under the same relative path:

`vdo/vdo-utilities`
