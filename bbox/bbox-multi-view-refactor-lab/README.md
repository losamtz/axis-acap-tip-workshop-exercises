# Bbox Multi View Refactor Lab Exercise

This exercise refactors the multi-view bbox animation to reuse one persistent bbox handle and let the GLib timer control the frame cadence.

`app/bbox_multi_view_lab.c` keeps the animation tick, cleanup helper, signal handler, and drawing logic in place so the exercise can focus on application lifecycle and persistent bbox resource management.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = bbox gio-2.0 glib-2.0
```

## Step 2: Add bbox access to manifest.json

Open `app/manifest.json`.

After `schemaVersion`, add the `resources` block below. Remember to add a comma after the `schemaVersion` line and keep the comma after the closing brace of `resources`.

```json
"resources": {
    "dbus": {
        "requiredMethods": [
            "com.axis.Graphics2.*",
            "com.axis.Overlay2.*"
        ]
    },
    "linux": {
        "user": {
            "groups": ["video"]
        }
    }
},
```

This gives the app access to the graphics and overlay D-Bus APIs and the `video` Linux group required by the bbox API.

## Step 3: Create the GLib main loop

Open `app/bbox_multi_view_lab.c`.

Paste this where the file says `TODO 1`:

```c
loop = g_main_loop_new(NULL, FALSE);
if (!loop)
    panic("Failed to create GMainLoop");
```

This creates the main loop that keeps the animation timer running.

## Step 4: Register signal handlers

Paste this where the file says `TODO 2`:

```c
g_unix_signal_add(SIGTERM, sig_handler, NULL);
g_unix_signal_add(SIGINT, sig_handler, NULL);
```

This lets the app stop cleanly when it receives `SIGTERM` or `SIGINT`.

## Step 5: Create the persistent bbox handle

Paste this where the file says `TODO 3`:

```c
g_bbox = bbox_new(4u, 1u, 2u, 3u, 4u);
if (!g_bbox)
    panic("bbox_new failed: %s", strerror(errno));
```

This creates one bbox handle for views 1, 2, 3, and 4. The app reuses this handle on every animation tick.

## Step 6: Start the animation

Paste this where the file says `TODO 4`:

```c
if (!bbox_video_output(g_bbox, true))
    panic("Failed enabling video-output: %s", strerror(errno));

g_timeout_add(TICK_MS, update_bbox, NULL);
g_main_loop_run(loop);
```

This enables overlay output, starts the timer-driven animation, and runs the main loop.

## Step 7: Clean up

Paste this where the file says `TODO 5`:

```c
clear_all();

if (g_bbox) {
    bbox_destroy(g_bbox);
    g_bbox = NULL;
}

if (loop) {
    g_main_loop_unref(loop);
    loop = NULL;
}
```

This clears the overlay and releases the persistent bbox and GLib resources.

## Build

From this example directory:

```sh
docker build --tag bbox-multi-view-refactor-lab --build-arg ARCH=aarch64 .
docker cp $(docker create bbox-multi-view-refactor-lab):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `bbox/bbox-multi-view-refactor-lab` in `axis-acap-tip-workshop`.
