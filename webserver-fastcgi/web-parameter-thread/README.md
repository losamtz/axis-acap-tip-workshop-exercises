# Web Parameter Thread Exercise

This exercise is based on `webserver-fastcgi/web-parameter-thread` from the complete `axis-acap-tip-workshop` repository.

`app/web_parameter_thread.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Review manifest configuration

This example uses manifest entries for `configuration`, `resources`, `httpConfig`, `paramConfig`, `settingPage`. Review `app/manifest.json` before building and keep these entries aligned with the README workflow.

## Step 2: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 gio-2.0 fcgi axparameter jansson
```

## Step 3: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
FCGX_Request req;
    char* socket_path = NULL;
    int status;
    int sock;
    GError* error = NULL;

    openlog(APP_NAME, LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Starting %s FastCGI app", APP_NAME);

    // Init AXParameter

    handle = ax_parameter_new(APP_NAME, &error);
    if (!handle) panic("ax_parameter_new failed: %s", error ? error->message : "unknown");

    // Ensure parameters exist (string metadata)
    pthread_mutex_lock(&handle_mtx);
    if (!add_if_missing("IpAddress", "192.168.0.90", "string"))
        panic("Failed to add IpAddress");
    if (!add_if_missing("Port", "8080", "string"))
        panic("Failed to add Port");
    pthread_mutex_unlock(&handle_mtx);

    socket_path = getenv(FCGI_SOCKET_NAME);
    syslog(LOG_INFO, "Socket: %s\n", socket_path);

    if (!socket_path) {
        syslog(LOG_ERR, "Failed to get environment variable FCGI_SOCKET_NAME");
        return EXIT_FAILURE;
    }
    // Init libraries
    status = FCGX_Init();
    if (status != 0) {
        syslog(LOG_INFO, "FCGX_Init failed");
        return status;
    }

    sock = FCGX_OpenSocket(socket_path, 5);
    chmod(socket_path, S_IRWXU | S_IRWXG | S_IRWXO);

    status = FCGX_InitRequest(&req, sock, 0);
```

## Step 4: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
if(status != 0) {
        panic("FCGX_InitRequest failed");
    }
    syslog(LOG_INFO, "FCGX_InitRequest succeeded");

    // Accept/handle loop
    while (FCGX_Accept_r(&req) == 0) {
        handle_request(&req);
        FCGX_Finish_r(&req);
    }

    ax_parameter_free(handle);
    closelog();
    return 0;
```

## Build

From this example directory:

```sh
docker build --tag web-parameter-thread --build-arg ARCH=aarch64 .
docker cp $(docker create web-parameter-thread):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `webserver-fastcgi/web-parameter-thread` in `axis-acap-tip-workshop`.
