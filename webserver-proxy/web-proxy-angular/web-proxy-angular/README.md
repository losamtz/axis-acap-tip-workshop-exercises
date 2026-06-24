# Web Proxy Angular Exercise

This exercise packages the Angular UI from `../acap-angular-ui/` together with a CivetWeb reverse-proxy backend.

`app/web_proxy_angular.c` keeps the JSON helpers, AXParameter helpers, CivetWeb handlers, and signal handler in place. Complete the TODOs in `main()` with the snippets below.

The important frontend/backend flow is:

- Angular is built with `baseHref: "./"` and `<base href="./">`
- the ACAP manifest exposes `index.html` as the settings page
- the manifest maps `api` to `http://localhost:2001`
- the C backend registers `/`, `/local/web_proxy/api/info`, and `/local/web_proxy/api/param`
- the Angular client calls `/local/web_proxy/api`

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 gio-2.0 jansson axparameter
```

## Step 2: Check the Angular base href

Open `../acap-angular-ui/angular.json`.

In `projects.acap-angular-ui.architect.build.options`, keep this option:

```json
"options": {
    "baseHref": "./",
    "polyfills": ["zone.js"],
    "browser": "src/main.ts"
}
```

Open `../acap-angular-ui/src/index.html` and confirm the matching base tag:

```html
<base href="./">
```

This keeps the compiled assets relative to `index.html` when the camera serves the ACAP settings page.

## Step 3: Add runtime access to manifest.json

Open `app/manifest.json`.

After `schemaVersion`, add:

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

## Step 4: Add the settings page, reverse proxy, and parameters

Inside `acapPackageConf`, after the `setup` block, add:

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

## Step 5: Initialize logging and signals

Open `app/web_proxy_angular.c`.

Paste this where the file says `TODO 1`:

```c
openlog(APP_NAME, LOG_PID, LOG_USER);
syslog(LOG_INFO, "Starting %s CivetWeb reverse-proxy backend", APP_NAME);

signal(SIGTERM, on_signal);
signal(SIGINT, on_signal);
```

## Step 6: Create the AXParameter handle

Paste this where the file says `TODO 2`:

```c
GError* error = NULL;
handle = ax_parameter_new(APP_NAME, &error);
if (!handle) {
    panic("ax_parameter_new failed: %s", error ? error->message : "unknown error");
}
```

## Step 7: Start the CivetWeb server

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

## Step 8: Register the Angular and API handlers

Paste this where the file says `TODO 4`:

```c
mg_set_request_handler(ctx, "/", RootHandler, NULL);
mg_set_request_handler(ctx, "/local/web_proxy/api/info", InfoHandler, NULL);
mg_set_request_handler(ctx, "/local/web_proxy/api/param", ParamHandler, NULL);
```

`RootHandler` serves `app/html/index.html`. The API handler paths intentionally match the Angular client base path `/local/web_proxy/api`.

## Step 9: Keep the server running

Paste this where the file says `TODO 5`:

```c
while (running) {
    sleep(1);
}
```

## Step 10: Clean up

Paste this where the file says `TODO 6`:

```c
mg_stop(ctx);
ax_parameter_free(handle);
closelog();
return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag web-proxy-angular --build-arg ARCH=aarch64 .
docker cp $(docker create web-proxy-angular):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `webserver-proxy/web-proxy-angular/web-proxy-angular` in `axis-acap-tip-workshop`.
