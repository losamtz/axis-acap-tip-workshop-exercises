# Larod Client Exercise

This exercise is based on `larod/larod-client` from the complete `axis-acap-tip-workshop` repository.

`app/larod_client_tool.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
/* Choose between { LOG_INFO, LOG_CRIT, LOG_WARNING, LOG_ERR }*/
    syslog(LOG_INFO, "Larod client tool to enable ssh developer mode, started!");
```

## Build

From this example directory:

```sh
docker build --tag larod-client --build-arg ARCH=aarch64 .
docker cp $(docker create larod-client):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `larod/larod-client` in `axis-acap-tip-workshop`.
