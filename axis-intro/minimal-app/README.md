# Minimal App Exercise

This exercise is based on `axis-intro/minimal-app` from the complete `axis-acap-tip-workshop` repository.

`app/axis_intro_minimal.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
openlog("axis_intro_minimal", LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Axis intro minimal app started");

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    while (running)
        sleep(1);

    syslog(LOG_INFO, "Axis intro minimal app stopped");
    closelog();
    return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag minimal-app --build-arg ARCH=aarch64 .
docker cp $(docker create minimal-app):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `axis-intro/minimal-app` in `axis-acap-tip-workshop`.
