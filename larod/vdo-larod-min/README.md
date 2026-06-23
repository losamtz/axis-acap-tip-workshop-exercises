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

Paste this where the file says `TODO 1`:

```c
larodConnection* conn = NULL;
larodModel* inf_model = NULL;
larodModel* pp_model = NULL;
larodTensor** inf_outputs = NULL;
larodTensor** pp_outputs = NULL;
larodJobRequest* pp_job = NULL;
larodJobRequest* inf_job = NULL;
size_t num_inf_outputs = 0;
size_t pp_num_outputs = 0;
int model_fd = -1;
unsigned int model_pitch = 0;
output_buf_t out_bufs[2] = {
    {.fd = -1, .data = MAP_FAILED},
    {.fd = -1, .data = MAP_FAILED},
};
tracked_input_t tracked[5] = {0};
VdoStream* vdo_stream = NULL;
unsigned int vdo_w = 0;
unsigned int vdo_h = 0;
unsigned int vdo_pitch = 0;
unsigned int vdo_nbr_bufs = 0;
VdoFormat vdo_format = 0;
bool vdo_is_dmabuf = false;
GError* vdo_error = NULL;

openlog("vdo_larod_min", LOG_PID | LOG_CONS, LOG_USER);
signal(SIGTERM, on_signal);
signal(SIGINT, on_signal);
syslog(LOG_INFO, "Starting vdo_larod_min");
```

Paste this where the file says `TODO 2`:

```c
conn = larod_connect();
inf_model = load_inference_model(conn, &model_fd);
read_model_input_size(conn, inf_model, &model_pitch);
```

Paste this where the file says `TODO 3`:

```c
inf_outputs = create_output_tensors(conn, inf_model, out_bufs, &num_inf_outputs);
syslog(LOG_INFO, "Model has %zu output tensors", num_inf_outputs);

bool rgb_backend = backend_supports_rgb(DEVICE_NAME);
vdo_stream = create_vdo_stream(rgb_backend,
                               &vdo_w,
                               &vdo_h,
                               &vdo_pitch,
                               &vdo_nbr_bufs,
                               &vdo_is_dmabuf,
                               &vdo_format);
```

Paste this where the file says `TODO 4`:

```c
bool need_pp = false;
if (rgb_backend) {
    need_pp = (vdo_w != MODEL_WIDTH || vdo_h != MODEL_HEIGHT);
    if (!need_pp) {
        syslog(LOG_INFO, "Preprocessing NO: VDO delivers RGB at expected resolution %ux%u", vdo_w, vdo_h);
    }
} else {
    need_pp = true;
    syslog(LOG_INFO, "Preprocessing YES: backend needs NV12 to RGB conversion");
}

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

Paste this where the file says `TODO 5`:

```c
create_input_tensors(tracked, vdo_nbr_bufs, vdo_w, vdo_h, vdo_pitch, vdo_format);

if (!vdo_stream_start(vdo_stream, &vdo_error)) {
    PANIC("vdo_stream_start: %s", vdo_error ? vdo_error->message : "unknown error");
}

int poll_fd = vdo_stream_get_fd(vdo_stream, &vdo_error);
if (poll_fd < 0) {
    PANIC("vdo_stream_get_fd: %s", vdo_error ? vdo_error->message : "unknown error");
}
struct pollfd pfd = {.fd = poll_fd, .events = POLLIN};

syslog(LOG_INFO, "Entering main inference loop");
```

Paste this where the file says `TODO 6`:

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
        if (!pp_job) {
            pp_job = larodCreateJobRequest(pp_model, input, 1, pp_outputs, pp_num_outputs, NULL, &error);
            if (!pp_job) {
                PANIC("larodCreateJobRequest(pp): %s", error ? error->msg : "unknown error");
            }
        } else {
            larodSetJobRequestInputs(pp_job, input, 1, &error);
        }
        if (!larodRunJob(conn, pp_job, &error)) {
            PANIC("larodRunJob(pp): %s", error ? error->msg : "unknown error");
        }
        input = pp_outputs;
    }

    if (!inf_job) {
        inf_job = larodCreateJobRequest(inf_model, input, 1, inf_outputs, num_inf_outputs, NULL, &error);
        if (!inf_job) {
            PANIC("larodCreateJobRequest(inference): %s", error ? error->msg : "unknown error");
        }
    } else if (!need_pp) {
        larodSetJobRequestInputs(inf_job, input, 1, &error);
    }
    if (!larodRunJob(conn, inf_job, &error)) {
        PANIC("larodRunJob(inference): %s", error ? error->msg : "unknown error");
    }

    syslog(LOG_INFO, "Inference completed");
    if (!vdo_stream_buffer_unref(vdo_stream, &vdo_buf, &vdo_error)) {
        PANIC("buffer_unref: %s", vdo_error ? vdo_error->message : "unknown error");
    }
}
```

Paste this where the file says `TODO 7`:

```c
if (pp_job) larodDestroyJobRequest(&pp_job);
if (inf_job) larodDestroyJobRequest(&inf_job);

for (unsigned int i = 0; i < vdo_nbr_bufs && i < 5; i++) {
    larodError* error = NULL;
    if (tracked[i].tensors) {
        larodDestroyTensors(conn, &tracked[i].tensors, 1, &error);
    }
    if (tracked[i].duped_fd >= 0) {
        close(tracked[i].duped_fd);
    }
}

larodError* error = NULL;
if (pp_outputs) larodDestroyTensors(conn, &pp_outputs, pp_num_outputs, &error);
if (inf_outputs) larodDestroyTensors(conn, &inf_outputs, num_inf_outputs, &error);
if (pp_model) larodDestroyModel(&pp_model);
if (inf_model) larodDestroyModel(&inf_model);

for (size_t i = 0; i < num_inf_outputs && i < 2; i++) {
    if (out_bufs[i].data != MAP_FAILED) {
        munmap(out_bufs[i].data, out_bufs[i].size);
    }
}

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
docker build --tag vdo-larod-min --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-larod-min):/opt/app ./build
```

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).
