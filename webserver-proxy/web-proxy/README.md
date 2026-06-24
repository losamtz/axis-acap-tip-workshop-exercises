# Web Proxy Exercise

This exercise exposes a small JSON API through the camera reverse proxy. The ACAP app runs a local CivetWeb server on port `2001`, and the camera forwards `/local/web_proxy/api/...` requests to it.

`app/web_proxy.c` keeps the JSON helpers, AXParameter helpers, CivetWeb handlers, and signal handler in place. Complete the TODOs in `main()` with the snippets below.

The important request flow is:

- the manifest maps `api` to `http://localhost:2001`
- the app starts CivetWeb on `PORT`
- `mg_set_request_handler()` registers the root, info, and parameter handlers
- `InfoHandler` reads AXParameter values and returns JSON
- `ParamHandler` parses JSON, updates AXParameter values, and returns JSON

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 gio-2.0 jansson axparameter
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

This gives the app the runtime access it needs for the admin-only reverse-proxy API.

## Step 3: Add the web proxy and parameter configuration

Inside `acapPackageConf`, after the `setup` block, add the `configuration` block below. Remember to add a comma after the closing brace of `setup`.

```json
"configuration": {
    "settingPage": "index.html",
    "reverseProxy": [
        {
            "apiPath": "api",
            "target": "http://localhost:2001",
            "access": "admin"
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

This packages the settings page, forwards `/local/web_proxy/api/...` to the local server, and creates the parameters used by the handlers.

## Step 4: Initialize logging and signals

Open `app/web_proxy.c`.

Paste this where the file says `TODO 1`:

```c
openlog(APP_NAME, LOG_PID, LOG_USER);
syslog(LOG_INFO, "Starting %s CivetWeb reverse-proxy backend", APP_NAME);

signal(SIGTERM, on_signal);
signal(SIGINT, on_signal);
```

This prepares logging and lets the app stop cleanly when the package is stopped.

## Step 5: Create the AXParameter handle

Paste this where the file says `TODO 2`:

```c
GError* error = NULL;
handle = ax_parameter_new(APP_NAME, &error);
if (!handle) {
    panic("ax_parameter_new failed: %s", error ? error->message : "unknown error");
}
```

The request handlers use this global handle to read and update `MulticastAddress` and `MulticastPort`.

## Step 6: Start the CivetWeb server

Paste this where the file says `TODO 3`:

```c
const char* opts[] = {
    "listening_ports", PORT,
    "request_timeout_ms", "10000",
    "error_log_file", "error.log",
    0
};

mg_init_library(0);

struct mg_callbacks cb;
memset(&cb, 0, sizeof(cb));
cb.begin_request = cb_begin_request;

struct mg_context* ctx = mg_start(&cb, NULL, opts);
if (!ctx) {
    panic("Failed to start CivetWeb on port %s", PORT);
}

syslog(LOG_INFO, "CivetWeb listening on port %s", PORT);
```

This starts the embedded HTTP server that the camera reverse proxy forwards to.

## Step 7: Register request handlers

Paste this where the file says `TODO 4`:

```c
mg_set_request_handler(ctx, "/", RootHandler, NULL);
mg_set_request_handler(ctx, "/local/web_proxy/api/info", InfoHandler, NULL);
mg_set_request_handler(ctx, "/local/web_proxy/api/param", ParamHandler, NULL);
```

The root handler serves `html/index.html`; the API handlers serve JSON.

## Step 8: Keep the server running

Paste this where the file says `TODO 5`:

```c
while (running) {
    sleep(1);
}
```

CivetWeb handles requests in the background while the main thread waits for `SIGTERM` or `SIGINT`.

## Step 9: Clean up

Paste this where the file says `TODO 6`:

```c
mg_stop(ctx);
ax_parameter_free(handle);
closelog();
return EXIT_SUCCESS;
```

This stops the local server and releases the AXParameter handle.

## Build

From this example directory:

```sh
docker build --tag web-proxy --build-arg ARCH=aarch64 .
docker cp $(docker create web-proxy):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `webserver-proxy/web-proxy` in `axis-acap-tip-workshop`.
