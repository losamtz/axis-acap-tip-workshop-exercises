# Onvif Request Exercise

This exercise is based on `vapix/onvif-request` from the complete `axis-acap-tip-workshop` repository.

`app/onvif_request.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Review manifest configuration

This example uses manifest entries for `resources`. Review `app/manifest.json` before building and keep these entries aligned with the README workflow.

## Step 2: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = glib-2.0 gio-2.0 libcurl
```

## Step 3: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
// GError *error = NULL;
    char *app_name = basename(argv[0]);
    // Initialize the mutex

    // Open syslog for logging
    openlog(app_name, LOG_PID, LOG_USER);

    int ret = EXIT_SUCCESS;
    syslog(LOG_INFO, "Starting %s", app_name);

    CURL *handle = curl_easy_init();

    syslog(LOG_INFO, "Initializing CURL handle ...");

    if (!handle)
    {

        syslog(LOG_ERR, "Failed to initialize CURL");
        ret = EXIT_FAILURE;
        goto clean_up;
    }

    char *credentials = retrieve_onvif_credentials("random_user");
    syslog(LOG_INFO, "[main] - Retrieving VAPIX credentials ... %s", credentials ? "success" : "failure");

    if (!credentials)
    {
        syslog(LOG_ERR, "Failed to retrieve VAPIX credentials");
        ret = EXIT_FAILURE;
        goto clean_up;
    }
    syslog(LOG_INFO, "VAPIX credentials retrieved successfully");

    char *response = set_onvif_properties(handle, credentials);
    syslog(LOG_INFO, "[main] - Setting ONVIF properties ... %s", response ? "success" : "failure");

    if (!response)
    {
        syslog(LOG_ERR, "Failed to send ONVIF request");
        ret = EXIT_FAILURE;
        g_free(response);
        goto clean_up;
    }
```

## Step 4: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
g_free(response);
    g_free(credentials);
clean_up:

    // Cleanup
    syslog(LOG_INFO, "Cleaning up resources ...");

    curl_easy_cleanup(handle);

    syslog(LOG_INFO, "Unregistering callbacks and freeing parameter handle ...");

    syslog(LOG_INFO, "Closing syslog ...");
    closelog();
    return ret;
```

## Build

From this example directory:

```sh
docker build --tag onvif-request --build-arg ARCH=aarch64 .
docker cp $(docker create onvif-request):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `vapix/onvif-request` in `axis-acap-tip-workshop`.
