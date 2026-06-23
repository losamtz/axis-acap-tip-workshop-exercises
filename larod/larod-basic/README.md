# Larod Basic Exercise

This exercise introduces the minimal live-camera larod flow: connect to larod, load a model, create an RGB VDO stream at the model input size, and run inference.

`app/larod_basic.c` contains the helper functions for connecting to larod and loading the model. Complete the TODOs in `main()` with the snippets below.

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

## Step 3: Initialize larod and load the model

Open `app/larod_basic.c`.

Paste this where the file says `TODO 1`:

```c
larodConnection* conn = NULL;
larodError* error = NULL;
int model_fd = -1;

larodModel* model = NULL;
larodTensor** out_tensors = NULL;
larodJobRequest* jobs[2] = {NULL, NULL};

size_t num_out = 0;
void* out_data[2] = {NULL, NULL};
size_t out_size[2] = {0, 0};

VdoStream* stream = NULL;
GError* vdo_err = NULL;

larodTensor** in_tensors[2] = {NULL, NULL};
int duped_fds[2] = {-1, -1};
int tracked_vdo_fds[2] = {-1, -1};

openlog("larod_basic", LOG_PID | LOG_CONS, LOG_USER);
signal(SIGTERM, on_signal);
signal(SIGINT, on_signal);

conn = larod_connect();
model = load_inference_model(conn, &model_fd);
```

This creates the variables used by the rest of the flow, installs signal handlers, connects to larod, and loads the model.

## Step 4: Read model metadata and allocate output tensors

Paste this where the file says `TODO 2`:

```c
size_t num_in = 0;
larodTensor** tmp_in = larodAllocModelInputs(conn, model, 0, &num_in, NULL, &error);
if (!tmp_in || num_in == 0) {
    PANIC("larodAllocModelInputs: %s", error ? error->msg : "unknown error");
}

const larodTensorDims* dims = larodGetTensorDims(tmp_in[0], &error);
if (!dims || dims->len != 4) {
    PANIC("Expected 4D input tensor");
}
unsigned int h = dims->dims[1];
unsigned int w = dims->dims[2];

const larodTensorPitches* pitches = larodGetTensorPitches(tmp_in[0], &error);
if (!pitches || pitches->len != 4) {
    PANIC("larodGetTensorPitches: %s", error ? error->msg : "unknown error");
}
unsigned int model_pitch = pitches->pitches[2];

syslog(LOG_INFO, "Model input: %ux%u pitch=%u", w, h, model_pitch);
larodDestroyTensors(conn, &tmp_in, num_in, &error);

out_tensors = larodAllocModelOutputs(conn,
                                     model,
                                     LAROD_FD_PROP_READWRITE | LAROD_FD_PROP_MAP,
                                     &num_out,
                                     NULL,
                                     &error);
if (!out_tensors) {
    PANIC("larodAllocModelOutputs: %s", error ? error->msg : "unknown error");
}

for (size_t i = 0; i < num_out && i < 2; i++) {
    int fd = larodGetTensorFd(out_tensors[i], &error);
    larodGetTensorFdSize(out_tensors[i], &out_size[i], &error);
    out_data[i] = mmap(NULL, out_size[i], PROT_READ, MAP_SHARED, fd, 0);
    if (out_data[i] == MAP_FAILED) {
        PANIC("mmap output[%zu]: %s", i, strerror(errno));
    }
}
```

This asks larod what the model input expects, then allocates mmap-able output tensors so the app can read inference results.

## Step 5: Create the matching RGB VDO stream

Paste this where the file says `TODO 3`:

```c
VdoMap* settings = vdo_map_new();
vdo_map_set_uint32(settings, "channel", 1);
vdo_map_set_uint32(settings, "format", VDO_FORMAT_RGB);
vdo_map_set_uint32(settings, "buffer.count", 2);
vdo_map_set_double(settings, "framerate", 30.0);
vdo_map_set_string(settings, "image.fit", "scale");

VdoPair32u res = {.w = w, .h = h};
vdo_map_set_pair32u(settings, "resolution", res);

stream = vdo_stream_new(settings, NULL, &vdo_err);
g_object_unref(settings);
if (!stream) {
    PANIC("vdo_stream_new: %s", vdo_err ? vdo_err->message : "unknown error");
}

VdoMap* info = vdo_stream_get_info(stream, &vdo_err);
if (!info) {
    PANIC("vdo_stream_get_info: %s", vdo_err ? vdo_err->message : "unknown error");
}
unsigned int vdo_w = vdo_map_get_uint32(info, "width", 0);
unsigned int vdo_h = vdo_map_get_uint32(info, "height", 0);
unsigned int vdo_pitch = vdo_map_get_uint32(info, "pitch", 0);
VdoFormat vdo_fmt = vdo_map_get_uint32(info, "format", 0);
g_object_unref(info);

if (vdo_fmt != VDO_FORMAT_RGB || vdo_w != w || vdo_h != h) {
    PANIC("VDO stream does not match model input: got fmt=%u %ux%u, expected RGB %ux%u",
          vdo_fmt,
          vdo_w,
          vdo_h,
          w,
          h);
}

if (!vdo_stream_start(stream, &vdo_err)) {
    PANIC("vdo_stream_start: %s", vdo_err ? vdo_err->message : "unknown error");
}
syslog(LOG_INFO, "VDO stream started: RGB %ux%u pitch=%u", vdo_w, vdo_h, vdo_pitch);
```

This creates a blocking RGB stream that matches the model input. In this basic example there is no preprocessing step.

## Step 6: Create VDO-backed larod input tensors

Paste this where the file says `TODO 4`:

```c
for (int i = 0; i < 2; i++) {
    in_tensors[i] = larodCreateTensors(1, &error);
    if (!in_tensors[i]) {
        PANIC("larodCreateTensors: %s", error ? error->msg : "unknown error");
    }

    larodTensor* tensor = in_tensors[i][0];
    larodSetTensorDataType(tensor, LAROD_TENSOR_DATA_TYPE_UINT8, &error);
    larodSetTensorLayout(tensor, LAROD_TENSOR_LAYOUT_NHWC, &error);
    larodBuildTensorDims(tensor, LAROD_TENSOR_LAYOUT_NHWC, vdo_w, vdo_h, 3, &error);
    larodBuildTensorPitches(tensor, LAROD_TENSOR_LAYOUT_NHWC, vdo_pitch, vdo_h, 3, &error);
    larodSetTensorFdProps(tensor, LAROD_FD_PROP_MAP | LAROD_FD_PROP_DMABUF, &error);
}
```

This creates tensor descriptors for VDO buffers. The image memory still belongs to VDO; the fd is attached when a frame arrives.

## Step 7: Run the inference loop

Paste this where the file says `TODO 5`:

```c
while (running) {
    VdoBuffer* buf = vdo_stream_get_buffer(stream, &vdo_err);
    if (!buf) {
        continue;
    }

    int vdo_fd = vdo_buffer_get_fd(buf);
    int slot = -1;
    for (int i = 0; i < 2; i++) {
        if (tracked_vdo_fds[i] == vdo_fd) {
            slot = i;
            break;
        }
    }

    if (slot < 0) {
        for (int i = 0; i < 2; i++) {
            if (tracked_vdo_fds[i] == -1) {
                slot = i;
                break;
            }
        }
        if (slot < 0) {
            PANIC("No free VDO buffer tracking slot");
        }

        duped_fds[slot] = dup(vdo_fd);
        if (duped_fds[slot] < 0) {
            PANIC("dup: %s", strerror(errno));
        }

        larodTensor* tensor = in_tensors[slot][0];
        larodSetTensorFd(tensor, duped_fds[slot], &error);
        larodSetTensorFdOffset(tensor, vdo_buffer_get_offset(buf), &error);
        larodSetTensorFdSize(tensor, vdo_buffer_get_capacity(buf), &error);
        if (!larodTrackTensor(conn, tensor, &error)) {
            PANIC("larodTrackTensor: %s", error ? error->msg : "unknown error");
        }
        tracked_vdo_fds[slot] = vdo_fd;
    }

    if (!jobs[slot]) {
        jobs[slot] = larodCreateJobRequest(model, in_tensors[slot], 1, out_tensors, num_out, NULL, &error);
        if (!jobs[slot]) {
            PANIC("larodCreateJobRequest: %s", error ? error->msg : "unknown error");
        }
    }

    if (!larodRunJob(conn, jobs[slot], &error)) {
        PANIC("larodRunJob: %s", error ? error->msg : "unknown error");
    }

    syslog(LOG_INFO, "Inference completed");
    if (!vdo_stream_buffer_unref(stream, &buf, &vdo_err)) {
        PANIC("buffer_unref: %s", vdo_err ? vdo_err->message : "unknown error");
    }
}
```

This waits for camera frames, tracks each VDO buffer fd once, creates the larod job request, and runs inference.

## Step 8: Clean up resources

Paste this where the file says `TODO 6`:

```c
for (int i = 0; i < 2; i++) {
    if (jobs[i]) {
        larodDestroyJobRequest(&jobs[i]);
    }
}

for (int i = 0; i < 2; i++) {
    if (in_tensors[i]) {
        larodDestroyTensors(conn, &in_tensors[i], 1, &error);
    }
    if (duped_fds[i] >= 0) {
        close(duped_fds[i]);
    }
}

for (size_t i = 0; i < num_out && i < 2; i++) {
    if (out_data[i] && out_data[i] != MAP_FAILED) {
        munmap(out_data[i], out_size[i]);
    }
}

if (out_tensors) {
    larodDestroyTensors(conn, &out_tensors, num_out, &error);
}
if (model) {
    larodDestroyModel(&model);
}
if (model_fd >= 0) {
    close(model_fd);
}
if (stream) {
    vdo_stream_stop(stream);
    g_object_unref(stream);
}
if (conn) {
    larodDisconnect(&conn, &error);
}

closelog();
return EXIT_SUCCESS;
```

This releases all resources owned or referenced by the app.

## Build

```sh
docker build --tag larod-basic --build-arg ARCH=aarch64 .
docker cp $(docker create larod-basic):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
