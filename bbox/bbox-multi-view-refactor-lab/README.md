# Bbox Multi View Refactor Lab Exercise

This exercise is based on `bbox/bbox-multi-view-refactor-lab` from the complete `axis-acap-tip-workshop` repository.

`app/bbox_multi_view_lab.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Review manifest configuration

This example uses manifest entries for `resources`. Review `app/manifest.json` before building and keep these entries aligned with the README workflow.

## Step 2: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = bbox gio-2.0 glib-2.0
```

## Step 3: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
openlog(NULL, LOG_PID | LOG_CONS, LOG_USER);

    // Create main loop
    loop = g_main_loop_new(NULL, FALSE);
    if (!loop) panic("Failed to create GMainLoop");

    // Handle signals
    g_unix_signal_add(SIGTERM, sig_handler, NULL);
    g_unix_signal_add(SIGINT, sig_handler, NULL);

    // Create a persistent bbox handle once.
    // Here we target 4 views (1..4). Adjust to your device layout.
    g_bbox = bbox_new(4u, 1u, 2u, 3u, 4u);
    if (!g_bbox) panic("bbox_new failed: %s", strerror(errno));
```

## Step 4: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
// Optional: choose normalized coordinate space (uncomment if needed)
    // bbox_coordinates_frame_normalized(g_bbox);
    // or bbox_coordinates_scene_normalized(g_bbox);

    // Prime the OSD
    if (!bbox_video_output(g_bbox, true))
        panic("Failed enabling video-output: %s", strerror(errno));

    // Start animation timer (no blocking sleeps inside update loop)
    g_timeout_add(TICK_MS, update_bbox, NULL);

    // Run
    g_main_loop_run(loop);

    // Shutdown: clear what we drew and destroy resources
    clear_all();

    if (g_bbox) {
        bbox_destroy(g_bbox);
        g_bbox = NULL;
    }

    if (loop) {
        g_main_loop_unref(loop);
        loop = NULL;
    }

    return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag bbox-multi-view-refactor-lab --build-arg ARCH=aarch64 .
docker cp $(docker create bbox-multi-view-refactor-lab):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `bbox/bbox-multi-view-refactor-lab` in `axis-acap-tip-workshop`.
