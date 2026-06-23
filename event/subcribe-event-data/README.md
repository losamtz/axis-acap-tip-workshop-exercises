# Subcribe Event Data Exercise

This exercise is based on `event/subcribe-event-data` from the complete `axis-acap-tip-workshop` repository.

`app/subscribe_event_data.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 axevent
```

## Step 2: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
GMainLoop* main_loop          = NULL;
    AXEventHandler* event_handler = NULL;
    guint subscription            = 0;

    // Set up the user logging to syslog
    openlog(NULL, LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Started logging from subscribe event application");

    // Event handler
    event_handler = ax_event_handler_new();
    subscription  = send_data_event_subscription(event_handler);

    // Main loop
    main_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(main_loop);
```

## Step 3: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
// Cleanup event handler
    ax_event_handler_unsubscribe(event_handler, subscription, NULL);
    ax_event_handler_free(event_handler);

    // Free g_main_loop
    g_main_loop_unref(main_loop);
```

## Build

From this example directory:

```sh
docker build --tag subcribe-event-data --build-arg ARCH=aarch64 .
docker cp $(docker create subcribe-event-data):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `event/subcribe-event-data` in `axis-acap-tip-workshop`.
