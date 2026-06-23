# Vdo Larod Min Exercise

This exercise builds a production-shaped minimal VDO-to-larod loop with non-blocking VDO, `poll`, optional preprocessing, tracked VDO buffers, and reusable job requests.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 gio-unix-2.0 liblarod vdostream
```

## Step 2: Add larod and video access to the manifests

Open `app/manifest.json.artpec8` and `app/manifest.json.artpec9`.

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

## Step 3: Build the pipeline in main

Open `app/vdo_larod_min.c`.

Complete the TODOs in order:

- `TODO 1`: declare local resources and initialize logging/signals
- `TODO 2`: connect, load model, and read input metadata
- `TODO 3`: allocate mmaped outputs and create the backend-aware VDO stream
- `TODO 4`: decide whether cpu-proc preprocessing is needed
- `TODO 5`: create input tensor descriptors and start non-blocking VDO
- `TODO 6`: poll, get buffers, track fds, run preprocessing/inference, and return buffers
- `TODO 7`: release jobs, tensors, models, streams, fds, mmap regions, and larod connection

## Build

```sh
docker build --tag vdo-larod-min --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-larod-min):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
