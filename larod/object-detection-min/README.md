# Object Detection Min Exercise

This exercise completes the full camera-to-object-detection path: VDO frame, optional preprocessing, larod inference, SSD postprocessing, and bbox overlay.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = bbox gio-2.0 gio-unix-2.0 liblarod vdostream
```

## Step 2: Add larod, video, and bbox access to manifest.json

Open `app/manifest.json`.

After `schemaVersion`, add:

```json
"resources": {
    "dbus": {
        "requiredMethods": [
            "com.axis.Graphics2.*",
            "com.axis.Overlay2.*"
        ]
    },
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

## Step 3: Build the detection pipeline in main

Open `app/object_detection_min.c`.

Complete the TODOs in order:

- `TODO 1`: declare resources and initialize logging/signals
- `TODO 2`: connect to larod, load model, read input metadata, and mmap outputs
- `TODO 3`: create VDO stream and optional cpu-proc preprocessing
- `TODO 4`: create input tensors and bbox overlay resources
- `TODO 5`: poll frames, track buffers, run preprocessing/inference, and postprocess SSD outputs
- `TODO 6`: draw detections with bbox and return each VDO buffer
- `TODO 7`: release bbox, larod, VDO, mmap, tensor, model, and fd resources

## Build

```sh
docker build --tag object-detection-min --build-arg ARCH=aarch64 .
docker cp $(docker create object-detection-min):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
