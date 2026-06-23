# Larod Basic Exercise

This exercise introduces the minimal live-camera larod flow: connect to larod, load a model, create an RGB VDO stream at the model input size, and run inference.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 gio-unix-2.0 liblarod vdostream
```

## Step 2: Add larod and video access to manifest.json

Open `app/manifest.json`.

After `schemaVersion`, add:

```json
"resources": {
    "linux": {
        "user": {
            "groups": ["video"]
        }
    },
    "deepLearningProcessor": {
        "enabled": true,
        "required": true
    }
},
```

## Step 3: Connect and load the model

Open `app/larod_basic.c`.

At `TODO 1`, initialize logging/signals, call `larod_connect()`, and call `load_inference_model(...)`.

## Step 4: Read metadata and allocate outputs

At `TODO 2`, allocate temporary model inputs to read input dimensions/pitch, then allocate and mmap output tensors.

## Step 5: Create the matching VDO stream

At `TODO 3`, create a blocking RGB VDO stream using the model width and height.

## Step 6: Create input tensors

At `TODO 4`, create larod input tensor descriptors for the VDO buffers with `UINT8`, `NHWC`, model-sized dimensions, VDO pitch, and DMA-BUF fd properties.

## Step 7: Run inference and clean up

At `TODO 5`, get VDO buffers, attach/track their fds as larod input tensors, create or reuse a job request, and run inference.

At `TODO 6`, destroy tensors, jobs, model, connection, mmap regions, VDO stream, duplicated fds, and the model fd.

## Build

```sh
docker build --tag larod-basic --build-arg ARCH=aarch64 .
docker cp $(docker create larod-basic):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
