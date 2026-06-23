# Object Detection Min Exercise

This exercise is based on `larod/object-detection-min` from the complete `axis-acap-tip-workshop` repository.

`app/object_detection_min.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Review manifest configuration

This example uses manifest entries for `resources`. Review `app/manifest.json` before building and keep these entries aligned with the README workflow.

## Step 2: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = bbox gio-2.0 gio-unix-2.0 liblarod vdostream
```

## Step 3: Add main setup snippet

Paste this into `main()` at the next TODO position:

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

## Step 4: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
/*
     * STEP 1-4 - Prepare larod and the model.
     *
     * At the end of this block we know the model input size and have CPU-visible
     * output buffers ready for postprocessing.
     */
    conn = larod_connect();
    inf_model = load_inference_model(conn, &model_fd);
    read_model_input_size(conn, inf_model, &model_pitch);
    inf_outputs = create_output_tensors(conn, inf_model, out_bufs, MAX_OUTPUT_TENSORS, &num_inf_outputs);

    /*
     * STEP 5 - Open the camera stream.
     *
     * The requested format depends on backend capability, but the values used
     * below are the actual stream values returned by VDO.
     */
    bool rgb_backend = backend_supports_rgb(DEVICE_NAME);
    vdo_stream = create_vdo_stream(rgb_backend,
                                   &vdo_w,
                                   &vdo_h,
                                   &vdo_pitch,
                                   &vdo_nbr_bufs,
                                   &vdo_is_dmabuf,
                                   &vdo_format);
```

## Step 5: Add main runtime flow snippet

Paste this into `main()` at the next TODO position:

```c
/*
     * STEP 6 - Decide whether preprocessing is needed.
     *
     * We need preprocessing if the backend cannot consume RGB directly, or if
     * the VDO stream dimensions do not match the model input dimensions.
     */
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

## Step 6: Add main processing loop snippet

Paste this into `main()` at the next TODO position:

```c
/*
     * STEP 7 - Create larod input tensor descriptors for each possible VDO
     * buffer. The real fd is attached lazily when a frame arrives.
     */
    create_input_tensors(tracked, vdo_nbr_bufs, vdo_w, vdo_h, vdo_pitch, vdo_format);

    /*
     * BBox setup
     *
     * bbox draws rectangles in the camera view. The postprocess function uses
     * normalized coordinates from the SSD model, so the boxes scale with the
     * displayed frame.
     */
    bbox = setup_bbox(VDO_CHANNEL);
    if (!bbox) {
        PANIC("setup_bbox failed");
    }
```

## Step 7: Add main cleanup snippet

Paste this into `main()` at the next TODO position:

```c
/*
     * STEP 9 - Start the stream and poll for frames.
     *
     * The stream is non-blocking, so poll waits until VDO has a frame ready.
     */
    if (!vdo_stream_start(vdo_stream, &vdo_error)) {
        PANIC("vdo_stream_start: %s", vdo_error ? vdo_error->message : "unknown error");
    }

    int poll_fd = vdo_stream_get_fd(vdo_stream, &vdo_error);
    if (poll_fd < 0) {
        PANIC("vdo_stream_get_fd: %s", vdo_error ? vdo_error->message : "unknown error");
    }

    struct pollfd pfd = {.fd = poll_fd, .events = POLLIN};
    syslog(LOG_INFO, "Entering inference loop");

    while (running) {
        larodError* error = NULL;
        int ret;
```

## Step 8: Add main workflow part 6 snippet

Paste this into `main()` at the next TODO position:

```c
/* STEP 9a - Wait until the VDO stream has a frame available. */
        do {
            ret = poll(&pfd, 1, -1);
        } while (ret == -1 && errno == EINTR);
        if (ret < 0) {
            PANIC("poll: %s", strerror(errno));
        }

        /* STEP 9b - Fetch one VDO buffer from the stream. */
        VdoBuffer* vdo_buf = vdo_stream_get_buffer(vdo_stream, &vdo_error);
        if (!vdo_buf) {
            if (g_error_matches(vdo_error, VDO_ERROR, VDO_ERROR_NO_DATA)) {
                g_clear_error(&vdo_error);
                continue;
            }
            PANIC("vdo_stream_get_buffer: %s", vdo_error ? vdo_error->message : "unknown error");
        }
```

## Step 9: Add main workflow part 7 snippet

Paste this into `main()` at the next TODO position:

```c
/*
         * STEP 8 - Track the frame buffer if this fd is new.
         *
         * input now points to the larod tensor array describing this frame.
         */
        int slot = track_vdo_buffer(conn, tracked, vdo_nbr_bufs, vdo_buf, vdo_is_dmabuf);
        larodTensor** input = tracked[slot].tensors;

        /*
         * STEP 10 - Run preprocessing, only when needed.
         *
         * The preprocessing job consumes the VDO tensor and writes RGB/resized
         * output tensors. The job object is created once, then its input tensor
         * is updated for each new VDO buffer.
         */
        if (need_pp) {
            if (!pp_job_request) {
                pp_job_request = larodCreateJobRequest(pp_model,
                                                       input,
                                                       1,
                                                       pp_outputs,
                                                       pp_num_outputs,
                                                       NULL,
                                                       &error);
                if (!pp_job_request) {
                    PANIC("larodCreateJobRequest(pp): %s", error ? error->msg : "unknown error");
                }
            } else {
                larodSetJobRequestInputs(pp_job_request, input, 1, &error);
            }

            if (!larodRunJob(conn, pp_job_request, &error)) {
                PANIC("larodRunJob(pp): %s", error ? error->msg : "unknown error");
            }
        }
```

## Step 10: Add main workflow part 8 snippet

Paste this into `main()` at the next TODO position:

```c
/*
         * STEP 11 - Run inference.
         *
         * If preprocessing ran, inference consumes pp_outputs. Otherwise it
         * consumes the raw VDO tensor directly.
         */
        larodTensor** inf_input = need_pp ? pp_outputs : input;
        size_t inf_input_n = need_pp ? pp_num_outputs : 1;

        if (!inf_job_request) {
            inf_job_request = larodCreateJobRequest(inf_model,
                                                    inf_input,
                                                    inf_input_n,
                                                    inf_outputs,
                                                    num_inf_outputs,
                                                    NULL,
                                                    &error);
            if (!inf_job_request) {
                PANIC("larodCreateJobRequest(inf): %s", error ? error->msg : "unknown error");
            }
        } else if (!need_pp) {
            larodSetJobRequestInputs(inf_job_request, inf_input, inf_input_n, &error);
        }

        if (!larodRunJob(conn, inf_job_request, &error)) {
            PANIC("larodRunJob(inf): %s", error ? error->msg : "unknown error");
        }
```

## Step 11: Add main workflow part 9 snippet

Paste this into `main()` at the next TODO position:

```c
/*
         * STEP 12 - Parse detections and draw boxes.
         *
         * The four mapped output tensors are read as floats. postprocess.c
         * filters by score and commits rectangles through bbox.
         */
        if (num_inf_outputs >= MAX_OUTPUT_TENSORS) {
            float confidence_threshold = (float)threshold / 100.0f;
            if (!parse_and_postprocess_output_tensors(bbox, out_bufs, confidence_threshold)) {
                syslog(LOG_ERR, "Failed to postprocess output tensors");
            }
        }
```

## Step 12: Add main workflow part 10 snippet

Paste this into `main()` at the next TODO position:

```c
/*
         * STEP 13 - Return the VDO buffer.
         *
         * VDO keeps ownership of frame buffers. Once this frame has been used,
         * unref it so VDO can recycle it for future frames.
         */
        if (!vdo_stream_buffer_unref(vdo_stream, &vdo_buf, &vdo_error)) {
            if (!vdo_error_is_expected(&vdo_error)) {
                PANIC("buffer_unref: %s", vdo_error ? vdo_error->message : "unknown error");
            }
            g_clear_error(&vdo_error);
        }
    }
```

## Step 13: Add main workflow part 11 snippet

Paste this into `main()` at the next TODO position:

```c
/*
     * STEP 14 - Cleanup.
     *
     * Stop VDO, unmap output buffers, destroy larod jobs/tensors/models, close
     * fds, and remove the bbox handle.
     */
    syslog(LOG_INFO, "Shutting down");

    if (vdo_stream) {
        vdo_stream_stop(vdo_stream);
        g_object_unref(vdo_stream);
    }

    for (size_t i = 0; i < num_inf_outputs; i++) {
        if (out_bufs[i].data != MAP_FAILED) {
            munmap(out_bufs[i].data, out_bufs[i].size);
        }
    }

    larodDestroyJobRequest(&pp_job_request);
    larodDestroyJobRequest(&inf_job_request);

    larodError* cleanup_error = NULL;
    for (unsigned int i = 0; i < MAX_TRACKED_BUFFERS; i++) {
        if (tracked[i].tensors) {
            larodDestroyTensors(conn, &tracked[i].tensors, 1, &cleanup_error);
        }
        if (tracked[i].duped_fd >= 0) {
            close(tracked[i].duped_fd);
        }
    }

    if (pp_outputs) {
        larodDestroyTensors(conn, &pp_outputs, pp_num_outputs, &cleanup_error);
    }
    if (inf_outputs) {
        larodDestroyTensors(conn, &inf_outputs, num_inf_outputs, &cleanup_error);
    }
```

## Step 14: Add main workflow part 12 snippet

Paste this into `main()` at the next TODO position:

```c
larodDestroyModel(&pp_model);
    larodDestroyModel(&inf_model);
    larodDisconnect(&conn, &cleanup_error);
    larodClearError(&cleanup_error);

    if (model_fd >= 0) {
        close(model_fd);
    }
    if (bbox) {
        bbox_destroy(bbox);
    }
    g_clear_error(&vdo_error);
    closelog();

    return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag object-detection-min --build-arg ARCH=aarch64 .
docker cp $(docker create object-detection-min):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `larod/object-detection-min` in `axis-acap-tip-workshop`.
