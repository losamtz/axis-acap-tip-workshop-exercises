# Larod Client Exercise

This small exercise creates an ACAP package that logs a startup message. It is useful when preparing developer access before running manual larod client experiments with model/input/output files.

## Step 1: Write a startup log message

Open `app/larod_client_tool.c`.

Paste this where the file says `TODO 1`:

```c
syslog(LOG_INFO, "Larod client tool to enable ssh developer mode, started!");
```

## Build

```sh
docker build --tag larod-client --build-arg ARCH=aarch64 .
docker cp $(docker create larod-client):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
