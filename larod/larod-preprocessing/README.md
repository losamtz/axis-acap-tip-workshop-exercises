# Larod Preprocessing Exercise

This exercise adds cpu-proc preprocessing to the VDO-to-larod flow so camera frames can be resized or converted before inference.

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

## Step 3: Connect to larod

Open `app/larod_preprocessing.c`.

At `TODO 1`, initialize logging/signals and connect with `larodConnect`.

## Step 4: Load model and read metadata

At `TODO 2`, open the model file, get the inference device, load the model, and read model input size/pitch.

## Step 5: Prepare outputs and VDO

At `TODO 3`, allocate and mmap inference output tensors.

At `TODO 4`, create the VDO stream and read back the actual width, height, pitch, and format.

## Step 6: Configure preprocessing

At `TODO 5`, compare VDO format/size with model requirements. If they differ, create a cpu-proc preprocessing model with a `larodMap` describing input and output format, size, and row pitch.

## Step 7: Run and clean up

At `TODO 6`, create input tensors, track VDO buffers, run preprocessing when needed, run inference, return buffers, and release larod/VDO resources.

## Build

```sh
docker build --tag larod-preprocessing --build-arg ARCH=aarch64 .
docker cp $(docker create larod-preprocessing):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
