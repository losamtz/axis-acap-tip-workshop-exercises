# Larod Client Exercise

This exercise is based on the corresponding complete example in `axis-acap-tip-workshop`.
The source file `app/larod_client_tool.c` has been reduced to a small TODO skeleton.

Your task is to rebuild the application flow by pasting the snippet below into `app/larod_client_tool.c`.
The snippet is intentionally kept in the README so you can read the sequence before editing the C file.

## What to do

1. Open `app/larod_client_tool.c`.
2. Replace the skeleton implementation with the code from **Implementation snippet** below.
3. Read through the code and identify the API setup, runtime loop, and cleanup flow.
4. Build the package with the commands in **Build**.


## Implementation snippet

Paste this into `app/larod_client_tool.c`:

```c
/**
 * Copyright (C) 2021, Axis Communications AB, Lund, Sweden
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * - larod_client_tool -
 *
 * This application writes "Larod client tool to enable ssh developer mode, started!" to the syslog.
 * It is only to create an ACAP user to create access to ssh user in developer mode.
 */

#include <syslog.h>

/***** Main function *********************************************************/

/**
 * brief Main function.
 *
 * This main function writes "Larod client tool to enable ssh developer mode" to the syslog.
 */
int main(void) {
    /* Choose between { LOG_INFO, LOG_CRIT, LOG_WARNING, LOG_ERR }*/
    syslog(LOG_INFO, "Larod client tool to enable ssh developer mode, started!");
}
```

## Build

From this example directory:

```sh
docker build --tag larod-client --build-arg ARCH=aarch64 .
docker cp $(docker create larod-client):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera from the Apps page or with your usual ACAP install flow.
If the application exposes HTTP endpoints or overlays, use the behavior described by the code comments and the parent module README to verify it.

## Reference

The complete version lives in the original `axis-acap-tip-workshop` repository under the same relative path:

`larod/larod-client`
