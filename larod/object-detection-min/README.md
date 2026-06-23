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

Paste this where the file says `TODO 1`:

```c
const int threshold = 50;
bbox_t* bbox = NULL;
larodConnection* conn = NULL;
larodModel* inf_model = NULL;
larodModel* pp_model = NULL;
larodTensor** inf_outputs = NULL;
larodTensor** pp_outputs = NULL;
larodJobRequest* pp_job_request = NULL;
larodJobRequest* inf_job_request = NULL;
size_t num_inf_outputs = 0;
size_t pp_num_outputs = 0;
int model_fd = -1;
unsigned int model_pitch = 0;
output_buf_t out_bufs[MAX_OUTPUT_TENSORS] = {
    {.fd = -1, .data = MAP_FAILED},
    {.fd = -1, .data = MAP_FAILED},
    {.fd = -1, .data = MAP_FAILED},
    {.fd = -1, .data = MAP_FAILED},
};
tracked_input_t tracked[MAX_TRACKED_BUFFERS] = {0};
VdoStream* vdo_stream = NULL;
unsigned int vdo_w = 0;
unsigned int vdo_h = 0;
unsigned int vdo_pitch = 0;
unsigned int vdo_nbr_bufs = 0;
VdoFormat vdo_format = 0;
bool vdo_is_dmabuf = false;
GError* vdo_error = NULL;

openlog("object_detection_min", LOG_PERROR | LOG_PID, LOG_USER);
signal(SIGTERM, on_signal);
signal(SIGINT, on_signal);
```

Paste this where the file says `TODO 2`:

```c
conn = larod_connect();
inf_model = load_inference_model(conn, &model_fd);
read_model_input_size(conn, inf_model, &model_pitch);
inf_outputs = create_output_tensors(conn,
                                    inf_model,
                                    out_bufs,
                                    MAX_OUTPUT_TENSORS,
                                    &num_inf_outputs);
```

Paste this where the file says `TODO 3`:

```c
bool rgb_backend = backend_supports_rgb(DEVICE_NAME);
vdo_stream = create_vdo_stream(rgb_backend,
                               &vdo_w,
                               &vdo_h,
                               &vdo_pitch,
                               &vdo_nbr_bufs,
                               &vdo_is_dmabuf,
                               &vdo_format);

bool need_pp = !rgb_backend || vdo_w != MODEL_WIDTH || vdo_h != MODEL_HEIGHT;
if (need_pp) {
    pp_model = setup_preprocessing(conn,
                                   vdo_format,
                                   vdo_w,
                                   vdo_h,
                                   vdo_pitch,
                                   model_pitch,
                                   &pp_outputs,
                                   &pp_num_outputs);
}
```

Paste this where the file says `TODO 4`:

```c
create_input_tensors(tracked, vdo_nbr_bufs, vdo_w, vdo_h, vdo_pitch, vdo_format);

bbox = setup_bbox(VDO_CHANNEL);
if (!bbox) {
    PANIC("setup_bbox failed");
}

if (!vdo_stream_start(vdo_stream, &vdo_error)) {
    PANIC("vdo_stream_start: %s", vdo_error ? vdo_error->message : "unknown error");
}

int poll_fd = vdo_stream_get_fd(vdo_stream, &vdo_error);
if (poll_fd < 0) {
    PANIC("vdo_stream_get_fd: %s", vdo_error ? vdo_error->message : "unknown error");
}
struct pollfd pfd = {.fd = poll_fd, .events = POLLIN};
syslog(LOG_INFO, "Entering inference loop");
```

Paste this where the file says `TODO 5`:

```c
while (running) {
    larodError* error = NULL;

    int ret;
    do {
        ret = poll(&pfd, 1, -1);
    } while (ret == -1 && errno == EINTR);
    if (ret < 0) {
        PANIC("poll: %s", strerror(errno));
    }

    VdoBuffer* vdo_buf = vdo_stream_get_buffer(vdo_stream, &vdo_error);
    if (!vdo_buf) {
        if (g_error_matches(vdo_error, VDO_ERROR, VDO_ERROR_NO_DATA)) {
            g_clear_error(&vdo_error);
            continue;
        }
        PANIC("vdo_stream_get_buffer: %s", vdo_error ? vdo_error->message : "unknown error");
    }

    int slot = track_vdo_buffer(conn, tracked, vdo_nbr_bufs, vdo_buf, vdo_is_dmabuf);
    larodTensor** input = tracked[slot].tensors;

    if (need_pp) {
        if (!pp_job_request) {
            pp_job_request = larodCreateJobRequest(pp_model, input, 1, pp_outputs, pp_num_outputs, NULL, &error);
            if (!pp_job_request) {
                PANIC("larodCreateJobRequest(pp): %s", error ? error->msg : "unknown error");
            }
        } else {
            larodSetJobRequestInputs(pp_job_request, input, 1, &error);
        }
        if (!larodRunJob(conn, pp_job_request, &error)) {
            PANIC("larodRunJob(pp): %s", error ? error->msg : "unknown error");
        }
        input = pp_outputs;
    }

    if (!inf_job_request) {
        inf_job_request = larodCreateJobRequest(inf_model, input, 1, inf_outputs, num_inf_outputs, NULL, &error);
        if (!inf_job_request) {
            PANIC("larodCreateJobRequest(inference): %s", error ? error->msg : "unknown error");
        }
    } else if (!need_pp) {
        larodSetJobRequestInputs(inf_job_request, input, 1, &error);
    }
    if (!larodRunJob(conn, inf_job_request, &error)) {
        PANIC("larodRunJob(inference): %s", error ? error->msg : "unknown error");
    }
```

Paste this where the file says `TODO 6`:

```c
    if (!parse_and_postprocess_output_tensors(bbox, out_bufs, (float)threshold / 100.0f)) {
        syslog(LOG_WARNING, "postprocess failed");
    }

    if (!vdo_stream_buffer_unref(vdo_stream, &vdo_buf, &vdo_error)) {
        PANIC("buffer_unref: %s", vdo_error ? vdo_error->message : "unknown error");
    }
}
```

Paste this where the file says `TODO 7`:

```c
larodError* error = NULL;

if (pp_job_request) larodDestroyJobRequest(&pp_job_request);
if (inf_job_request) larodDestroyJobRequest(&inf_job_request);

for (unsigned int i = 0; i < vdo_nbr_bufs && i < MAX_TRACKED_BUFFERS; i++) {
    if (tracked[i].tensors) {
        larodDestroyTensors(conn, &tracked[i].tensors, 1, &error);
    }
    if (tracked[i].duped_fd >= 0) {
        close(tracked[i].duped_fd);
    }
}

if (pp_outputs) larodDestroyTensors(conn, &pp_outputs, pp_num_outputs, &error);
if (inf_outputs) larodDestroyTensors(conn, &inf_outputs, num_inf_outputs, &error);
if (pp_model) larodDestroyModel(&pp_model);
if (inf_model) larodDestroyModel(&inf_model);

for (size_t i = 0; i < num_inf_outputs && i < MAX_OUTPUT_TENSORS; i++) {
    if (out_bufs[i].data != MAP_FAILED) {
        munmap(out_bufs[i].data, out_bufs[i].size);
    }
}

if (bbox) bbox_destroy(bbox);
if (model_fd >= 0) close(model_fd);
if (vdo_stream) {
    vdo_stream_stop(vdo_stream);
    g_object_unref(vdo_stream);
}
if (conn) larodDisconnect(&conn, &error);

closelog();
return EXIT_SUCCESS;
```

## Build

```sh
docker build --tag object-detection-min --build-arg ARCH=aarch64 .
docker cp $(docker create object-detection-min):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
