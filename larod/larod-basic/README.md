# Larod Basic Exercise

This exercise is based on `larod/larod-basic` from the complete `axis-acap-tip-workshop` repository.

`app/larod_basic.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Review manifest configuration

This example uses manifest entries for `resources`. Review `app/manifest.json` before building and keep these entries aligned with the README workflow.

## Step 2: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 gio-unix-2.0 liblarod vdostream
```

## Step 3: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
larodConnection* conn  = NULL;
    larodError*      error = NULL;
    int model_fd = -1;

    openlog("larod_basic", LOG_PID | LOG_CONS, LOG_USER);
    signal(SIGTERM, on_signal);
    signal(SIGINT,  on_signal);

    /* ── 1. Connect to larod ── */
    conn = larod_connect();

    /* ── 2. Load model ── */
    larodModel* model = load_inference_model(conn, &model_fd);

    /* ── 3. Get model input size ── */
    size_t num_in = 0;
    larodTensor** tmp_in = larodAllocModelInputs(conn, model, 0, &num_in, NULL, &error);
    const larodTensorDims* dims = larodGetTensorDims(tmp_in[0], &error);
    unsigned int h = dims->dims[1];
    unsigned int w = dims->dims[2];

    const larodTensorPitches* pitches = larodGetTensorPitches(tmp_in[0], &error);
    unsigned int model_pitch = pitches->pitches[2];

    syslog(LOG_INFO, "Model input: %ux%u pitch=%u", w, h, model_pitch);
    larodDestroyTensors(conn, &tmp_in, num_in, &error);
```

## Step 4: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
/* ── 4. Allocate output tensors + mmap ── */
    size_t num_out = 0;
    larodTensor** out_tensors = larodAllocModelOutputs(conn, model,
                                    LAROD_FD_PROP_READWRITE | LAROD_FD_PROP_MAP,
                                    &num_out, NULL, &error);
    void* out_data[2] = {NULL, NULL};
    for (size_t i = 0; i < num_out && i < 2; i++) {
        int fd = larodGetTensorFd(out_tensors[i], &error);
        size_t sz = 0;
        larodGetTensorFdSize(out_tensors[i], &sz, &error);
        out_data[i] = mmap(NULL, sz, PROT_READ, MAP_SHARED, fd, 0);
        if (out_data[i] == MAP_FAILED) {
            PANIC("mmap output[%zu]: %s", i, strerror(errno));
        }
    }
```

## Step 5: Add main runtime flow snippet

Paste this into `main()` at the next TODO position:

```c
/* ── 5. Create VDO stream (blocking, RGB, model resolution) ── */
    VdoMap* settings = vdo_map_new();
    vdo_map_set_uint32(settings, "channel", 1); // Using channel 1
    vdo_map_set_uint32(settings, "format", VDO_FORMAT_RGB);
    vdo_map_set_uint32(settings, "buffer.count", 2);
    vdo_map_set_double(settings, "framerate", 30.0);
    vdo_map_set_string(settings, "image.fit", "scale");
    VdoPair32u res = { .w = w, .h = h };
    vdo_map_set_pair32u(settings, "resolution", res);
    /* socket.blocking defaults to true — vdo_stream_get_buffer will block */

    GError* vdo_err = NULL;
    VdoStream* stream = vdo_stream_new(settings, NULL, &vdo_err);
    g_object_unref(settings);
    if (!stream) {
        PANIC("vdo_stream_new: %s", vdo_err->message);
    }

    VdoMap* info = vdo_stream_get_info(stream, &vdo_err);
    if (!info) {
        PANIC("vdo_stream_get_info: %s", vdo_err->message);
    }
    unsigned int vdo_w     = vdo_map_get_uint32(info, "width", 0);
    unsigned int vdo_h     = vdo_map_get_uint32(info, "height", 0);
    unsigned int vdo_pitch = vdo_map_get_uint32(info, "pitch", 0);
    VdoFormat    vdo_fmt   = vdo_map_get_uint32(info, "format", 0);
    g_object_unref(info);

    if (vdo_fmt != VDO_FORMAT_RGB || vdo_w != w || vdo_h != h) {
        PANIC("VDO stream does not match model input: got fmt=%u %ux%u, expected RGB %ux%u",
              vdo_fmt, vdo_w, vdo_h, w, h);
    }

    vdo_stream_start(stream, &vdo_err);
    syslog(LOG_INFO, "VDO stream started (blocking, RGB %ux%u pitch=%u)",
           vdo_w, vdo_h, vdo_pitch);
```

## Step 6: Add main processing loop snippet

Paste this into `main()` at the next TODO position:

```c
/* ── 6. Allocate input tensors (one per buffer) ── */
    larodTensor** in_tensors[2] = {NULL, NULL};
    int duped_fds[2] = {-1, -1};
    int tracked_vdo_fds[2] = {-1, -1};

    for(int i = 0; i < 2; i++) {
        duped_fds[i]       = -1;
        tracked_vdo_fds[i] = -1;

        in_tensors[i] = larodCreateTensors(1, &error);
        if (!in_tensors[i]) {
            PANIC("larodCreateTensors: %s", error->msg);
        }
        larodTensor* t = in_tensors[i][0];
        larodSetTensorDataType(t, LAROD_TENSOR_DATA_TYPE_UINT8, &error);
        larodSetTensorLayout(t, LAROD_TENSOR_LAYOUT_NHWC, &error);
        larodBuildTensorDims(t, LAROD_TENSOR_LAYOUT_NHWC, vdo_w, vdo_h, 3, &error);
        larodBuildTensorPitches(t, LAROD_TENSOR_LAYOUT_NHWC, vdo_pitch, vdo_h, 3, &error);
        larodSetTensorFdProps(t, LAROD_FD_PROP_MAP | LAROD_FD_PROP_DMABUF, &error);
    }
    syslog(LOG_INFO, "Created %d input tensors (NHWC RGB %ux%u pitch=%u)", 2, vdo_w, vdo_h, vdo_pitch);
```

## Step 7: Add main cleanup snippet

Paste this into `main()` at the next TODO position:

```c
/* ── 7. Inference job (created lazily) ── */
    larodJobRequest* job = NULL;

    /* ── 8. Main loop ── */
    while (running) {
        VdoBuffer* buf = vdo_stream_get_buffer(stream, &vdo_err);  /* blocks */
        if (!buf) continue;

        int vdo_fd = vdo_buffer_get_fd(buf);

        /* Find or create tracked slot for this buffer */
        int slot = -1;
        for (int i = 0; i < 2; i++) {
            if (tracked_vdo_fds[i] == vdo_fd) { slot = i; break; }
        }
        if (slot == -1) {
            /* First time seeing this buffer — set up tensor */
            for (int i = 0; i < 2; i++) {
                if (tracked_vdo_fds[i] == -1) { slot = i; break; }
            }
            if (slot < 0) {
                PANIC("No free tracking slots");
            }
```

## Step 8: Add main workflow part 6 snippet

Paste this into `main()` at the next TODO position:

```c
/* Tensors already created in step 6 — just bind the VDO buffer fd */
            int64_t offset = vdo_buffer_get_offset(buf);
            size_t cap     = vdo_buffer_get_capacity(buf);
            int duped      = dup(vdo_fd);
            if (duped < 0) {
                PANIC("dup: %s", strerror(errno));
            }

            larodTensor* t = in_tensors[slot][0];
            larodSetTensorFd(t, duped, &error);
            larodSetTensorFdOffset(t, offset, &error);
            larodSetTensorFdSize(t, cap, &error);
            larodTrackTensor(conn, t, &error);

            tracked_vdo_fds[slot] = vdo_fd;
            duped_fds[slot] = duped;
            syslog(LOG_INFO, "Tracked buffer slot %d (vdo_fd=%d)", slot, vdo_fd);
        }
```

## Step 9: Add main workflow part 7 snippet

Paste this into `main()` at the next TODO position:

```c
/* Create or update job */
        if (!job) {
            job = larodCreateJobRequest(model,
                                        in_tensors[slot], 1,
                                        out_tensors, num_out,
                                        NULL, &error);
        } else {
            larodSetJobRequestInputs(job, in_tensors[slot], 1, &error);
        }

        /* Run inference */
        if (larodRunJob(conn, job, &error)) {
            uint8_t* person = (uint8_t*)out_data[0];
            uint8_t* car    = (uint8_t*)out_data[1];
            syslog(LOG_INFO, "Person: %.1f%% — Car: %.1f%%",
                   *person / 2.55f, *car / 2.55f);
        }

        vdo_stream_buffer_unref(stream, &buf, &vdo_err);
    }
```

## Step 10: Add main workflow part 8 snippet

Paste this into `main()` at the next TODO position:

```c
/* ── 9. Cleanup ── */
    larodDestroyJobRequest(&job);
    for (int i = 0; i < 2; i++) {
        if (in_tensors[i]) larodDestroyTensors(conn, &in_tensors[i], 1, &error);
        if (duped_fds[i] >= 0) close(duped_fds[i]);
    }
    larodDestroyTensors(conn, &out_tensors, num_out, &error);
    larodDestroyModel(&model);
    larodDisconnect(&conn, &error);
    vdo_stream_stop(stream);
    g_object_unref(stream);
    if (model_fd >= 0) {
        close(model_fd);
    }

    syslog(LOG_INFO, "Done");
    closelog();
    return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag larod-basic --build-arg ARCH=aarch64 .
docker cp $(docker create larod-basic):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `larod/larod-basic` in `axis-acap-tip-workshop`.
