# Bbox Multi View Exercise

This exercise is based on `bbox/bbox-multi-view` from the complete `axis-acap-tip-workshop` repository.

`app/bbox_multi_view.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

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
GMainLoop *loop = NULL;
    openlog(NULL, LOG_PID | LOG_CONS, LOG_USER);
    loop = g_main_loop_new(NULL, FALSE);
    // Handle SIGTERM/SIGINT
    g_unix_signal_add(SIGTERM, signal_handler, loop);
    g_unix_signal_add(SIGINT, signal_handler, loop);

    // Add update loop every 100ms
    g_timeout_add(100, update_bbox, NULL);

    g_main_loop_run(loop);

    clear();

    return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag bbox-multi-view --build-arg ARCH=aarch64 .
docker cp $(docker create bbox-multi-view):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `bbox/bbox-multi-view` in `axis-acap-tip-workshop`.
