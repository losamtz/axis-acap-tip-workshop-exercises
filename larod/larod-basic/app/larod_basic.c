/**
 * larod_basic.c
 *
 * The simplest possible VDO + larod application.
 * Blocking VDO, no preprocessing, no tensor tracking, no poll().
 * ~100 lines of actual logic.
 *
 * Only works on backends that accept RGB directly (e.g. a9-dlpu-tflite).
 * VDO delivers RGB at the model's resolution, frames go straight to inference.
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <syslog.h>
#include <unistd.h>

#include "larod.h"
#include "vdo-buffer.h"
#include "vdo-error.h"
#include "vdo-frame.h"
#include "vdo-map.h"
#include "vdo-stream.h"
#include "vdo-types.h"

#include <glib.h>

#define DEVICE_NAME  "a9-dlpu-tflite" /* or "axis-a8-dlpu-tflite"      */
#define MODEL_PATH   "/usr/local/packages/larod_basic/model/model.tflite"

#define PANIC(fmt, ...)                                 \
    do {                                                \
        syslog(LOG_ERR, "FATAL: " fmt, ##__VA_ARGS__); \
        exit(EXIT_FAILURE);                             \
    } while (0)


static volatile sig_atomic_t running = 1;
static void on_signal(int s) { (void)s; running = 0; }


/* ══════════════════════════════════════════════
 *
 *  STEP 1 — CONNECT TO LAROD
 *
 * ══════════════════════════════════════════════ */
static larodConnection* larod_connect(void) {
    larodConnection* conn = NULL;
    larodError* error = NULL;

    if(!larodConnect(&conn, &error)) {
        PANIC("larodConnect: %s", error->msg);
    }
    syslog(LOG_INFO, "Connected to larod successfully");
    return conn;
}
/* ══════════════════════════════════════════════
 *
 *  STEP 2 — LOAD THE INFERENCE MODEL
 *
 *  Opens the .tflite file, selects the device
 *  (e.g. "a9-dlpu-tflite"), and loads the model.
 *
 * ══════════════════════════════════════════════ */
static larodModel* load_inference_model(larodConnection* conn, int* model_fd_out) {
    larodError*      error = NULL;

    int model_fd = open(MODEL_PATH, O_RDONLY);

    *model_fd_out = model_fd;

    const larodDevice* device = larodGetDevice(conn, DEVICE_NAME, 0, &error);
    larodModel* model = larodLoadModel(conn, model_fd, device,
                                       LAROD_ACCESS_PRIVATE, "", NULL, &error);
    if (!model) {
        PANIC("larodLoadModel: %s", error->msg);
    }
    syslog(LOG_INFO, "Model loaded successfully");
    return model;
}

int main(void) {
    /* TODO 1: Initialize logging, signals, larod connection, and model. */
    /* TODO 2: Read model input metadata and allocate output tensors. */
    /* TODO 3: Create the matching RGB VDO stream. */
    /* TODO 4: Create VDO-backed larod input tensors. */
    /* TODO 5: Run the blocking VDO-to-larod inference loop. */
    /* TODO 6: Clean up larod, VDO, mmap, and fd resources. */

    return 0;
}
