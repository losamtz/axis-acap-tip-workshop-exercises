# Dynamic Overlay Vapix Exercise

This exercise is based on `vapix/dynamic-overlay-vapix` from the complete `axis-acap-tip-workshop` repository.

`app/dynamic_overlay_vapix.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Review manifest configuration

This example uses manifest entries for `resources`. Review `app/manifest.json` before building and keep these entries aligned with the README workflow.

## Step 2: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 libcurl jansson
```

## Step 3: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
openlog(NULL, LOG_PID, LOG_USER);

    syslog(LOG_INFO, "Curl version %s", curl_version_info(CURLVERSION_NOW)->version);
    syslog(LOG_INFO, "Jansson version %s", JANSSON_VERSION);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* handle = curl_easy_init();

    char* credentials = retrieve_vapix_credentials("example-vapix-user");

    json_t* response = add_text(handle, credentials);

    char* debug_str = json_dumps(response, JSON_INDENT(2));
    syslog(LOG_INFO, "Full json response:\n%s", debug_str);

    syslog(LOG_INFO, "Camera: %s", response_data(response, "camera"));
    syslog(LOG_INFO, "Identity: %s", response_data(response, "identity"));

    free(debug_str);
    json_decref(response);
    free(credentials);
    curl_easy_cleanup(handle);
    curl_global_cleanup();
```

## Build

From this example directory:

```sh
docker build --tag dynamic-overlay-vapix --build-arg ARCH=aarch64 .
docker cp $(docker create dynamic-overlay-vapix):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `vapix/dynamic-overlay-vapix` in `axis-acap-tip-workshop`.
