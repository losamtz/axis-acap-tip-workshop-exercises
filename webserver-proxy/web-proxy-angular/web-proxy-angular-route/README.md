# Web Proxy Angular Route Exercise

This exercise is based on `webserver-proxy/web-proxy-angular/web-proxy-angular-route` from the complete `axis-acap-tip-workshop` repository.

`app/web_proxy_angular_route.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Review manifest configuration

This example uses manifest entries for `configuration`, `resources`, `reverseProxy`, `paramConfig`, `settingPage`. Review `app/manifest.json` before building and keep these entries aligned with the README workflow.

## Step 2: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 gio-2.0 jansson axparameter
```

## Step 3: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
openlog(APP_NAME, LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Starting %s (CivetWeb single-threaded)", APP_NAME);

    signal(SIGTERM, on_signal);
    signal(SIGINT,  on_signal);

    // Init AXParameter
    GError* error = NULL;
    handle = ax_parameter_new(APP_NAME, &error);

    if (!handle)
        panic("ax_parameter_new failed: %s", error ? error->message : "unknown");

    // Start CivetWeb in single-threaded mode
    const char* opts[] = {"listening_ports", PORT, "request_timeout_ms", "10000", "error_log_file", "error.log", 0};

    mg_init_library(0);

    struct mg_callbacks cb;
    memset(&cb, 0, sizeof(cb));
    cb.begin_request = cb_begin_request; // Log requests

    struct mg_context* ctx = mg_start(&cb, NULL, opts);

    if (!ctx)
        panic("Failed to start CivetWeb on %s", PORT);

    syslog(LOG_INFO, "CivetWeb listening on %s", PORT);
```

## Step 4: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
// Register handlers
    mg_set_request_handler(ctx, "/", RootHandler,  NULL);
    mg_set_request_handler(ctx, "/local/web_proxy/api/info",  InfoHandler,  NULL);
    mg_set_request_handler(ctx, "/local/web_proxy/api/param",  ParamHandler,  NULL);

    while (running)
        sleep(1);

    mg_stop(ctx);
    ax_parameter_free(handle);
    closelog();
    return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag web-proxy-angular-route --build-arg ARCH=aarch64 .
docker cp $(docker create web-proxy-angular-route):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `webserver-proxy/web-proxy-angular/web-proxy-angular-route` in `axis-acap-tip-workshop`.
