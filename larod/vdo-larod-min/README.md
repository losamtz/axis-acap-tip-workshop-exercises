# Vdo Larod Min Exercise

This exercise is based on `larod/vdo-larod-min` from the complete `axis-acap-tip-workshop` repository.

`app/vdo_larod_min.c` keeps the original headers, helper functions, callbacks, signal handling, and other support code. Complete only the TODOs in `main()` by pasting the snippets below in order.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 gio-unix-2.0 liblarod vdostream
```

## Step 2: Add main setup snippet

Paste this into `main()` at the next TODO position:

```c
/* ── Local variables ── */
    larodConnection*  conn            = NULL;
    larodModel*       inf_model       = NULL;
    larodModel*       pp_model        = NULL;
    larodTensor**     inf_outputs     = NULL;
    larodTensor**     pp_outputs      = NULL;
    larodJobRequest*  pp_job          = NULL;
    larodJobRequest*  inf_job         = NULL;
    size_t            num_inf_outputs = 0;
    size_t            pp_num_outputs  = 0;
    int               model_fd       = -1;
    unsigned int      model_pitch    = 0;
    output_buf_t      out_bufs[2]    = {{.fd=-1, .data=MAP_FAILED},
                                        {.fd=-1, .data=MAP_FAILED}};
    tracked_input_t   tracked[5]     = {0};   /* MAX_NBR_IMG_PROVIDER_BUFFERS = 5 */
    VdoStream*        vdo_stream     = NULL;
    unsigned int      vdo_w, vdo_h, vdo_pitch, vdo_nbr_bufs;
    VdoFormat         vdo_format;
    bool              vdo_is_dmabuf;
```

## Step 3: Add main configuration snippet

Paste this into `main()` at the next TODO position:

```c
/* ── Init ── */
    openlog("vdo_larod_min", LOG_PID | LOG_CONS, LOG_USER);
    signal(SIGTERM, on_signal);
    signal(SIGINT,  on_signal);
    syslog(LOG_INFO, "========== Starting vdo_larod_min ==========");

    /* ── Step 1: Connect to larod ── */
    conn = larod_connect();

    /* ── Step 2: Load inference model ── */
    inf_model = load_inference_model(conn, &model_fd);

    /* ── Step 3: Read what the model expects - input size ── */
    read_model_input_size(conn, inf_model, &model_pitch);
```

## Step 4: Add main runtime flow snippet

Paste this into `main()` at the next TODO position:

```c
/* ── Step 4: Create + mmap output tensors ── */
    inf_outputs = create_output_tensors(conn, inf_model, out_bufs, &num_inf_outputs);
    syslog(LOG_INFO, "Model has %zu output tensors", num_inf_outputs);

    /* ── Step 5: Determine backend capabilities */
    bool rgb_backend = backend_supports_rgb(DEVICE_NAME);

    /* ── Step 6: Create VDO stream ── */
    vdo_stream = create_vdo_stream(rgb_backend, &vdo_w, &vdo_h, &vdo_pitch, &vdo_nbr_bufs, &vdo_is_dmabuf, &vdo_format);

    /* ── Step 7: Do we need preprocessing? ── */
    bool need_pp;

    if(rgb_backend) {
        /* If the backend supports RGB, we only need PP if the resolution doesn't match */
        need_pp = (vdo_w != MODEL_WIDTH || vdo_h != MODEL_HEIGHT);
        if(!need_pp) {
            syslog(LOG_INFO, "Preprocessing NO: VDO delivers RGB at the expected resolution %ux%u → no preprocessing needed", vdo_w, vdo_h);
        }
    } else {
        /* If the backend doesn't support RGB and gives us NV12, we always need PP to convert formats */
        need_pp = true;
        syslog(LOG_INFO, "Preprocessing YES: VDO delivers NV12 but model needs RGB → preprocessing needed to convert formats");
    }

    if (need_pp) {
        pp_model = setup_preprocessing(conn, vdo_format, vdo_w, vdo_h, vdo_pitch, model_pitch,
                                       &pp_outputs, &pp_num_outputs);
    }
```

## Step 5: Add main processing loop snippet

Paste this into `main()` at the next TODO position:

```c
/* ── Step 8: Create input tensors (one per VDO buffer) ── */
    create_input_tensors(tracked, vdo_nbr_bufs, vdo_w, vdo_h, vdo_pitch, vdo_format);

    /* ── Step 9: Start VDO stream and enter main loop ── */
    GError* vdo_error = NULL;
    if (!vdo_stream_start(vdo_stream, &vdo_error)) {
        PANIC("vdo_stream_start: %s", vdo_error->message);
    }

    int poll_fd = vdo_stream_get_fd(vdo_stream, &vdo_error);
    if (poll_fd < 0) {
        PANIC("vdo_stream_get_fd: %s", vdo_error->message);
    }
    struct pollfd pfd = { .fd = poll_fd, .events = POLLIN };

    syslog(LOG_INFO, "Entering main inference loop");

    while (running) {
        larodError* error = NULL;
```

## Step 6: Add main cleanup snippet

Paste this into `main()` at the next TODO position:

```c
/* ── 9a: Wait for a frame ── */
        int ret;
        do {
            ret = poll(&pfd, 1, -1);
        } while (ret == -1 && errno == EINTR);
        if (ret < 0) PANIC("poll: %s", strerror(errno));

        /* ── 9b: Get the VDO buffer ── */
        VdoBuffer* vdo_buf = vdo_stream_get_buffer(vdo_stream, &vdo_error);
        if (!vdo_buf) {
            if (g_error_matches(vdo_error, VDO_ERROR, VDO_ERROR_NO_DATA)) {
                g_clear_error(&vdo_error);
                continue;
            }
            PANIC("vdo_stream_get_buffer: %s", vdo_error->message);
        }
```

## Step 7: Add main workflow part 6 snippet

Paste this into `main()` at the next TODO position:

```c
/* ── 9c: Track the buffer (first-time setup per VDO fd) ── */
        int slot = track_vdo_buffer(conn, tracked, vdo_nbr_bufs, vdo_buf, vdo_is_dmabuf);
        larodTensor** input = tracked[slot].tensors;

        /* ── 9d: Run preprocessing (if needed) ── */
        if (need_pp) {
            /* Lazy-create the preprocessing job request */
            if (!pp_job) {
                pp_job = larodCreateJobRequest(pp_model,
                                               input, 1,
                                               pp_outputs, pp_num_outputs,
                                               NULL, &error);
                if (!pp_job) PANIC("larodCreateJobRequest(pp): %s", error->msg);
            } else {
                larodSetJobRequestInputs(pp_job, input, 1, &error);
            }

            if (!larodRunJob(conn, pp_job, &error)) {
                PANIC("larodRunJob(pp): %s", error->msg);
            }
        }
```

## Step 8: Add main workflow part 7 snippet

Paste this into `main()` at the next TODO position:

```c
/* ── 9e: Run inference ── */
        /* Input is either the raw VDO tensor or the PP output */
        larodTensor** inf_input    = need_pp ? pp_outputs : input;
        size_t        inf_input_n  = need_pp ? pp_num_outputs : 1;

        if (!inf_job) {
            inf_job = larodCreateJobRequest(inf_model,
                                            inf_input, inf_input_n,
                                            inf_outputs, num_inf_outputs,
                                            NULL, &error);
            if (!inf_job) PANIC("larodCreateJobRequest(inf): %s", error->msg);
        }
        /* No need to update inputs — they don't change after PP is stable */

        if (!larodRunJob(conn, inf_job, &error)) {
            PANIC("larodRunJob(inf): %s", error->msg);
        }
```

## Step 9: Add main workflow part 8 snippet

Paste this into `main()` at the next TODO position:

```c
/* ── 9f: Read results ── */
        if (num_inf_outputs >= 2) {
            uint8_t* person = (uint8_t*)out_bufs[0].data;
            uint8_t* car    = (uint8_t*)out_bufs[1].data;
            syslog(LOG_INFO,
                   "Person: %.1f%% — Car: %.1f%%",
                   (float)*person / 2.55f,
                   (float)*car / 2.55f);
        }

        /* ── 9g: Return VDO buffer ── */
        if (!vdo_stream_buffer_unref(vdo_stream, &vdo_buf, &vdo_error)) {
            if (!vdo_error_is_expected(&vdo_error)) {
                PANIC("buffer_unref: %s", vdo_error->message);
            }
            g_clear_error(&vdo_error);
        }
    }
```

## Step 10: Add main workflow part 9 snippet

Paste this into `main()` at the next TODO position:

```c
/* ══════════════════════════════════════════════
     *  STEP 9 — CLEANUP
     * ══════════════════════════════════════════════ */
    syslog(LOG_INFO, "Shutting down...");

    /* Stop VDO */
    if (vdo_stream) {
        vdo_stream_stop(vdo_stream);
        g_object_unref(vdo_stream);
    }

    /* Unmap output buffers */
    for (int i = 0; i < 2; i++) {
        if (out_bufs[i].data != MAP_FAILED) munmap(out_bufs[i].data, out_bufs[i].size);
    }
```

## Step 11: Add main workflow part 10 snippet

Paste this into `main()` at the next TODO position:

```c
/* Destroy job requests */
    larodDestroyJobRequest(&pp_job);
    larodDestroyJobRequest(&inf_job);

    /* Destroy tracked input tensors */
    larodError* cerr = NULL;
    for (unsigned int i = 0; i < VDO_NUM_BUFFERS; i++) {
        if (tracked[i].tensors) {
            larodDestroyTensors(conn, &tracked[i].tensors, 1, &cerr);
        }
        if (tracked[i].duped_fd >= 0) close(tracked[i].duped_fd);
    }
```

## Step 12: Add main workflow part 11 snippet

Paste this into `main()` at the next TODO position:

```c
/* Destroy output tensors */
    if (pp_outputs)  larodDestroyTensors(conn, &pp_outputs,  pp_num_outputs,  &cerr);
    if (inf_outputs) larodDestroyTensors(conn, &inf_outputs, num_inf_outputs, &cerr);

    /* Destroy models */
    larodDestroyModel(&pp_model);
    larodDestroyModel(&inf_model);

    /* Disconnect */
    larodDisconnect(&conn, &cerr);
    larodClearError(&cerr);

    /* Close model file */
    if (model_fd >= 0) close(model_fd);

    syslog(LOG_INFO, "========== vdo_larod_min exited ==========");
    closelog();
    return EXIT_SUCCESS;
```

## Build

From this example directory:

```sh
docker build --tag vdo-larod-min --build-arg ARCH=aarch64 .
docker cp $(docker create vdo-larod-min):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `larod/vdo-larod-min` in `axis-acap-tip-workshop`.
