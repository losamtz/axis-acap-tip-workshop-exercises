# Send State With Data Exercise

This exercise is based on `event/send-events-types/send-state-with-data` from the complete `axis-acap-tip-workshop` repository.

`app/send_state_with_data.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 axevent gio-2.0 libcurl jansson
```

## Step 2: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
GMainLoop* main_loop  = NULL;
    guint start_value     = 0;

    // Set up the user logging to syslog
    openlog("send_state_with_data", LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Started logging from send event application");

    //Initialize the event handler
    app_data                = calloc(1, sizeof(AppData));
    app_data->event_handler = ax_event_handler_new();
    app_data->event_id      = setup_declaration(app_data->event_handler, &start_value);

    // main loop
    main_loop = g_main_loop_new( NULL, FALSE);

    g_main_loop_run(main_loop);

    /// Cleanup event handler
    ax_event_handler_undeclare(app_data->event_handler, app_data->event_id, NULL);
    ax_event_handler_free(app_data->event_handler);
    free(app_data);

    // Free g_main_loop
    g_main_loop_unref(main_loop);

    return 0;
```

## Build

From this example directory:

```sh
docker build --tag send-state-with-data --build-arg ARCH=aarch64 .
docker cp $(docker create send-state-with-data):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `event/send-events-types/send-state-with-data` in `axis-acap-tip-workshop`.
