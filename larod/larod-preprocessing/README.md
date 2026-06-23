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

Paste this where the file says `TODO 1`:

```c
larodConnection* conn = NULL;
larodError* error = NULL;
int model_fd = -1;

larodModel* model = NULL;
larodModel* pp_model = NULL;
larodTensor** out_tensors = NULL;
larodTensor** pp_outputs = NULL;
larodTensor** input_tensors[NUM_BUFFERS] = {NULL, NULL};
larodJobRequest* pp_jobs[NUM_BUFFERS] = {NULL, NULL};
larodJobRequest* inf_jobs[NUM_BUFFERS] = {NULL, NULL};

size_t num_out = 0;
size_t pp_num_out = 0;
void* out_data[2] = {NULL, NULL};
size_t out_size[2] = {0, 0};

VdoStream* stream = NULL;
GError* vdo_err = NULL;
int duped_fds[NUM_BUFFERS] = {-1, -1};
int tracked_vdo_fds[NUM_BUFFERS] = {-1, -1};

openlog("larod_preprocessing", LOG_PID | LOG_CONS, LOG_USER);
signal(SIGTERM, on_signal);
signal(SIGINT, on_signal);

if (!larodConnect(&conn, &error)) {
    syslog(LOG_ERR, "larodConnect: %s", error ? error->msg : "unknown error");
    return EXIT_FAILURE;
}
```

## Step 4: Load model and read metadata

Paste this where the file says `TODO 2`:

```c
model_fd = open(MODEL_PATH, O_RDONLY);
if (model_fd < 0) {
    syslog(LOG_ERR, "open model: %s", strerror(errno));
    return EXIT_FAILURE;
}

const larodDevice* device = larodGetDevice(conn, DEVICE_NAME, 0, &error);
model = larodLoadModel(conn, model_fd, device, LAROD_ACCESS_PRIVATE, "", NULL, &error);
if (!model) {
    syslog(LOG_ERR, "larodLoadModel: %s", error ? error->msg : "unknown error");
    return EXIT_FAILURE;
}

size_t tmp_num_in = 0;
larodTensor** tmp_in = larodAllocModelInputs(conn, model, 0, &tmp_num_in, NULL, &error);
const larodTensorDims* model_dims = larodGetTensorDims(tmp_in[0], &error);
unsigned int model_w = model_dims->dims[2];
unsigned int model_h = model_dims->dims[1];
const larodTensorPitches* model_pitches = larodGetTensorPitches(tmp_in[0], &error);
unsigned int model_pitch = model_pitches->pitches[2];
syslog(LOG_INFO, "Model input: %ux%u pitch=%u", model_w, model_h, model_pitch);
larodDestroyTensors(conn, &tmp_in, tmp_num_in, &error);
```

## Step 5: Prepare outputs and VDO

Paste this where the file says `TODO 3`:

```c
out_tensors = larodAllocModelOutputs(conn,
                                     model,
                                     LAROD_FD_PROP_READWRITE | LAROD_FD_PROP_MAP,
                                     &num_out,
                                     NULL,
                                     &error);
if (!out_tensors) {
    syslog(LOG_ERR, "larodAllocModelOutputs: %s", error ? error->msg : "unknown error");
    return EXIT_FAILURE;
}

for (size_t i = 0; i < num_out && i < 2; i++) {
    int fd = larodGetTensorFd(out_tensors[i], &error);
    larodGetTensorFdSize(out_tensors[i], &out_size[i], &error);
    out_data[i] = mmap(NULL, out_size[i], PROT_READ, MAP_SHARED, fd, 0);
    if (out_data[i] == MAP_FAILED) {
        syslog(LOG_ERR, "mmap output[%zu]: %s", i, strerror(errno));
        return EXIT_FAILURE;
    }
}
```

Paste this where the file says `TODO 4`:

```c
VdoMap* settings = vdo_map_new();
vdo_map_set_uint32(settings, "channel", VDO_CHANNEL);
vdo_map_set_uint32(settings, "format", VDO_FMT);
vdo_map_set_uint32(settings, "buffer.count", NUM_BUFFERS);
vdo_map_set_double(settings, "framerate", 2.0);
vdo_map_set_string(settings, "image.fit", IMAGE_FIT);
VdoPair32u res = {.w = VDO_WIDTH, .h = VDO_HEIGHT};
vdo_map_set_pair32u(settings, "resolution", res);

stream = vdo_stream_new(settings, NULL, &vdo_err);
g_object_unref(settings);
if (!stream) {
    syslog(LOG_ERR, "vdo_stream_new: %s", vdo_err ? vdo_err->message : "unknown error");
    return EXIT_FAILURE;
}

VdoMap* info = vdo_stream_get_info(stream, &vdo_err);
unsigned int vdo_w = vdo_map_get_uint32(info, "width", 0);
unsigned int vdo_h = vdo_map_get_uint32(info, "height", 0);
unsigned int vdo_pitch = vdo_map_get_uint32(info, "pitch", 0);
VdoFormat vdo_fmt = vdo_map_get_uint32(info, "format", 0);
g_object_unref(info);

syslog(LOG_INFO, "VDO stream: %ux%u pitch=%u fmt=%u", vdo_w, vdo_h, vdo_pitch, vdo_fmt);
```

## Step 6: Configure preprocessing

Paste this where the file says `TODO 5`:

```c
bool need_pp = (vdo_fmt != VDO_FORMAT_RGB || vdo_w != model_w || vdo_h != model_h);
syslog(LOG_INFO, "Preprocessing: %s", need_pp ? "YES" : "NO");

if (need_pp) {
    const char* input_format = vdo_fmt == VDO_FORMAT_YUV ? "nv12" : "rgb-interleaved";

    larodMap* map = larodCreateMap(&error);
    larodMapSetStr(map, "image.input.format", input_format, &error);
    larodMapSetIntArr2(map, "image.input.size", vdo_w, vdo_h, &error);
    larodMapSetInt(map, "image.input.row-pitch", vdo_pitch, &error);
    larodMapSetStr(map, "image.output.format", "rgb-interleaved", &error);
    larodMapSetIntArr2(map, "image.output.size", model_w, model_h, &error);
    larodMapSetInt(map, "image.output.row-pitch", model_pitch, &error);

    const larodDevice* pp_device = larodGetDevice(conn, "cpu-proc", 0, &error);
    pp_model = larodLoadModel(conn, -1, pp_device, LAROD_ACCESS_PRIVATE, "", map, &error);
    larodDestroyMap(&map);
    if (!pp_model) {
        syslog(LOG_ERR, "larodLoadModel(preprocessing): %s", error ? error->msg : "unknown error");
        return EXIT_FAILURE;
    }

    pp_outputs = larodAllocModelOutputs(conn,
                                        pp_model,
                                        LAROD_FD_PROP_READWRITE | LAROD_FD_PROP_MAP,
                                        &pp_num_out,
                                        NULL,
                                        &error);
}
```

## Step 7: Run and clean up

Paste this where the file says `TODO 6`:

```c
larodTensorLayout layout = vdo_fmt == VDO_FORMAT_YUV ? LAROD_TENSOR_LAYOUT_420SP : LAROD_TENSOR_LAYOUT_NHWC;

for (int i = 0; i < NUM_BUFFERS; i++) {
    input_tensors[i] = larodCreateTensors(1, &error);
    if (!input_tensors[i]) {
        syslog(LOG_ERR, "larodCreateTensors: %s", error ? error->msg : "unknown error");
        return EXIT_FAILURE;
    }

    larodTensor* input = input_tensors[i][0];
    larodSetTensorDataType(input, LAROD_TENSOR_DATA_TYPE_UINT8, &error);
    larodSetTensorLayout(input, layout, &error);
    larodBuildTensorDims(input, layout, vdo_w, vdo_h, 3, &error);
    larodBuildTensorPitches(input, layout, vdo_pitch, vdo_h, 3, &error);
    larodSetTensorFdProps(input, LAROD_FD_PROP_MAP | LAROD_FD_PROP_DMABUF, &error);
}

if (!vdo_stream_start(stream, &vdo_err)) {
    syslog(LOG_ERR, "vdo_stream_start: %s", vdo_err ? vdo_err->message : "unknown error");
    return EXIT_FAILURE;
}

while (running) {
    VdoBuffer* buf = vdo_stream_get_buffer(stream, &vdo_err);
    if (!buf) {
        continue;
    }

    int vdo_fd = vdo_buffer_get_fd(buf);
    int slot = -1;
    for (int i = 0; i < NUM_BUFFERS; i++) {
        if (tracked_vdo_fds[i] == vdo_fd) {
            slot = i;
            break;
        }
    }

    if (slot < 0) {
        for (int i = 0; i < NUM_BUFFERS; i++) {
            if (tracked_vdo_fds[i] == -1) {
                slot = i;
                break;
            }
        }
        if (slot < 0) {
            syslog(LOG_ERR, "No free VDO buffer tracking slot");
            vdo_stream_buffer_unref(stream, &buf, &vdo_err);
            continue;
        }

        duped_fds[slot] = dup(vdo_fd);
        if (duped_fds[slot] < 0) {
            syslog(LOG_ERR, "dup: %s", strerror(errno));
            vdo_stream_buffer_unref(stream, &buf, &vdo_err);
            continue;
        }

        larodTensor* input = input_tensors[slot][0];
        larodSetTensorFd(input, duped_fds[slot], &error);
        larodSetTensorFdOffset(input, vdo_buffer_get_offset(buf), &error);
        larodSetTensorFdSize(input, vdo_buffer_get_capacity(buf), &error);
        larodTrackTensor(conn, input, &error);
        tracked_vdo_fds[slot] = vdo_fd;
    }

    larodTensor** inference_input = input_tensors[slot];
    if (need_pp) {
        if (!pp_jobs[slot]) {
            pp_jobs[slot] = larodCreateJobRequest(pp_model, input_tensors[slot], 1, pp_outputs, pp_num_out, NULL, &error);
        }
        larodRunJob(conn, pp_jobs[slot], &error);
        inference_input = pp_outputs;
    }

    if (!inf_jobs[slot]) {
        inf_jobs[slot] = larodCreateJobRequest(model, inference_input, 1, out_tensors, num_out, NULL, &error);
    }
    larodRunJob(conn, inf_jobs[slot], &error);
    syslog(LOG_INFO, "Inference completed");
    if (!vdo_stream_buffer_unref(stream, &buf, &vdo_err)) {
        syslog(LOG_ERR, "buffer_unref: %s", vdo_err ? vdo_err->message : "unknown error");
        break;
    }
}

for (int i = 0; i < NUM_BUFFERS; i++) {
    if (pp_jobs[i]) larodDestroyJobRequest(&pp_jobs[i]);
    if (inf_jobs[i]) larodDestroyJobRequest(&inf_jobs[i]);
    if (input_tensors[i]) larodDestroyTensors(conn, &input_tensors[i], 1, &error);
    if (duped_fds[i] >= 0) close(duped_fds[i]);
}
if (pp_outputs) larodDestroyTensors(conn, &pp_outputs, pp_num_out, &error);
if (out_tensors) larodDestroyTensors(conn, &out_tensors, num_out, &error);
if (pp_model) larodDestroyModel(&pp_model);
if (model) larodDestroyModel(&model);
for (size_t i = 0; i < num_out && i < 2; i++) {
    if (out_data[i] && out_data[i] != MAP_FAILED) munmap(out_data[i], out_size[i]);
}
if (model_fd >= 0) close(model_fd);
if (stream) {
    vdo_stream_stop(stream);
    g_object_unref(stream);
}
if (conn) larodDisconnect(&conn, &error);
closelog();
return EXIT_SUCCESS;
```

## Build

```sh
docker build --tag larod-preprocessing --build-arg ARCH=aarch64 .
docker cp $(docker create larod-preprocessing):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
