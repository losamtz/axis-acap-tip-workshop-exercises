# Web Parameter Exercise

This exercise exposes a small JSON API through the camera web server with FastCGI. The API reads and updates AXParameter values used by the bundled settings page.

`app/web_parameter.c` keeps the JSON helpers, AXParameter helpers, and endpoint handler functions in place. Complete the TODOs by pasting the snippets below.

The important request flow is:

- the manifest maps `info-acap.cgi` and `param-acap.cgi` to FastCGI
- the app opens the FastCGI socket from `FCGI_SOCKET_NAME`
- `FCGX_Accept_r()` receives each request from the camera web server
- `route_request()` dispatches to `handle_info()` or `handle_param()`
- the handlers read or update AXParameter values and return JSON

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 gio-2.0 fcgi axparameter jansson
```

## Step 2: Add runtime access to manifest.json

Open `app/manifest.json`.

After `schemaVersion`, add the `resources` block below. Remember to add a comma after the `schemaVersion` line and keep the comma after the closing brace of `resources`.

```json
"resources": {
    "linux": {
        "user": {
            "groups": [
                "admin"
            ]
        }
    }
},
```

This gives the app the runtime access it needs for the admin-only FastCGI endpoints.

## Step 3: Add the web and parameter configuration

Inside `acapPackageConf`, after the `setup` block, add the `configuration` block below. Remember to add a comma after the closing brace of `setup`.

```json
"configuration": {
    "settingPage": "index.html",
    "httpConfig": [
        {
            "access": "admin",
            "name": "param-acap.cgi",
            "type": "fastCgi"
        },
        {
            "access": "admin",
            "name": "info-acap.cgi",
            "type": "fastCgi"
        }
    ],
    "paramConfig": [
        {
            "name": "MulticastAddress",
            "default": "224.0.0.1",
            "type": "string"
        },
        {
            "name": "MulticastPort",
            "default": "1024",
            "type": "string"
        }
    ]
}
```

This packages the settings page, exposes the CGI paths, and creates the parameters used by the handlers.

## Step 4: Route GET requests to the info handler

Open `app/web_parameter.c`.

Paste this where the file says `TODO 1`:

```c
if (strcmp(script, "/local/web_parameter/info-acap.cgi") == 0 && strcmp(method, "GET") == 0) {
    handle_info(req, h);
    return;
}
```

This maps the settings page read request to `handle_info()`.

## Step 5: Route POST requests to the parameter handler

Paste this where the file says `TODO 2`:

```c
if (strcmp(script, "/local/web_parameter/param-acap.cgi") == 0 && strcmp(method, "POST") == 0) {
    handle_param(req, h);
    return;
}
```

This maps JSON update requests to `handle_param()`.

## Step 6: Return JSON for unknown routes

Paste this where the file says `TODO 3`:

```c
write_json_header(req, 404);
FCGX_FPrintF(req->out, "{\"ok\":false,\"error\":\"Not found\"}");
```

This keeps all responses JSON, even when the path or method is unsupported.

## Step 7: Initialize logging and read the socket path

Paste this where the file says `TODO 4`:

```c
FCGX_Request req;
char* socket_path = NULL;
int status = 0;
int sock = -1;

openlog(APP_NAME, LOG_PID, LOG_DAEMON);
syslog(LOG_INFO, "Starting %s FastCGI app", APP_NAME);

socket_path = getenv(FCGI_SOCKET_NAME);
if (!socket_path) {
    syslog(LOG_ERR, "Failed to get environment variable FCGI_SOCKET_NAME");
    closelog();
    return EXIT_FAILURE;
}

syslog(LOG_INFO, "FastCGI socket: %s", socket_path);
```

The camera runtime provides `FCGI_SOCKET_NAME`; the app must open that socket for the web server to forward requests.

## Step 8: Initialize FastCGI and AXParameter

Paste this where the file says `TODO 5`:

```c
status = FCGX_Init();
if (status != 0) {
    syslog(LOG_ERR, "FCGX_Init failed: %d", status);
    closelog();
    return status;
}

AXParameter* handle = init_axparameter();
```

This initializes the FastCGI library and creates the AXParameter handle used by the endpoint handlers.

## Step 9: Open the FastCGI socket

Paste this where the file says `TODO 6`:

```c
sock = FCGX_OpenSocket(socket_path, 5);
if (sock < 0) {
    ax_parameter_free(handle);
    panic("FCGX_OpenSocket failed");
}

chmod(socket_path, S_IRWXU | S_IRWXG | S_IRWXO);

status = FCGX_InitRequest(&req, sock, 0);
if (status != 0) {
    ax_parameter_free(handle);
    panic("FCGX_InitRequest failed: %d", status);
}

syslog(LOG_INFO, "FastCGI request object initialized");
```

This starts the server side of the FastCGI connection and prepares one request object for the single-threaded loop.

## Step 10: Accept and handle requests

Paste this where the file says `TODO 7`:

```c
while (FCGX_Accept_r(&req) == 0) {
    syslog(LOG_INFO, "Accepted FastCGI request");

    route_request(&req, handle);
    FCGX_Finish_r(&req);
}
```

Each accepted request is routed to one of the handler functions and then finished before the next request is accepted.

## Step 11: Clean up

Paste this where the file says `TODO 8`:

```c
ax_parameter_free(handle);
syslog(LOG_INFO, "Exiting");
closelog();
return 0;
```

This releases the AXParameter handle and closes syslog.

## Build

From this example directory:

```sh
docker build --tag web-parameter --build-arg ARCH=aarch64 .
docker cp $(docker create web-parameter):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `webserver-fastcgi/web-parameter` in `axis-acap-tip-workshop`.
