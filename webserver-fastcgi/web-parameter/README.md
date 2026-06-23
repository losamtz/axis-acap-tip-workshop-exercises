# Web Parameter Exercise

This exercise is based on `webserver-fastcgi/web-parameter` from the complete `axis-acap-tip-workshop` repository.

`app/web_parameter.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

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
// Single request object reused in loop (single-threaded)
    FCGX_Request req;
    char* socket_path = NULL;
    int status;
    int sock;

    openlog(APP_NAME, LOG_PID, LOG_DAEMON);

    syslog(LOG_INFO, "Starting %s (single-process FastCGI)", APP_NAME);

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

    AXParameter* handle = init_axparameter();

    sock = FCGX_OpenSocket(socket_path, 5);
    chmod(socket_path, S_IRWXU | S_IRWXG | S_IRWXO);

    status = FCGX_InitRequest(&req, sock, 0);

    if(status != 0) {
        panic("FCGX_InitRequest failed");
    }
    syslog(LOG_INFO, "FCGX_InitRequest succeeded");
```

## Step 4: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
// Accept/handle loop
    while (FCGX_Accept_r(&req) == 0) {
        syslog(LOG_INFO, "FCGX_Accept_r OK");

        route_request(&req, handle);
        FCGX_Finish_r(&req);
    }

    // Cleanup
    ax_parameter_free(handle);
    syslog(LOG_INFO, "Exiting");
    closelog();
    return 0;
```

## Build

From this example directory:

```sh
docker build --tag web-parameter --build-arg ARCH=aarch64 .
docker cp $(docker create web-parameter):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `webserver-fastcgi/web-parameter` in `axis-acap-tip-workshop`.
