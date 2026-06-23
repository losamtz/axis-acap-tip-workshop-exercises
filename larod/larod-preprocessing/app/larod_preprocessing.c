/**
 * larod_preprocessing.c
 *
 * Minimal VDO + larod with preprocessing.
 * VDO delivers 720p. Preprocessing resizes to model resolution.
 * Blocking VDO, single file.
 *
 * Input tensors are ALWAYS created manually with larodCreateTensors,
 * matching the original vdo-larod example pattern.
 * They describe the VDO frame layout and are used as input to either:
 *   - the preprocessing model (if VDO size/format ≠ model)
 *   - the inference model directly (if VDO matches model exactly)
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
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

/* ── Configuration ── */
#define DEVICE_NAME  "a9-dlpu-tflite"
#define MODEL_PATH   "/usr/local/packages/larod_preprocessing/model/model.tflite"
#define VDO_WIDTH    640
#define VDO_HEIGHT   360
#define VDO_FMT      VDO_FORMAT_RGB    /* or VDO_FORMAT_YUV for NV12 */
#define IMAGE_FIT    "scale"
#define NUM_BUFFERS  2
#define VDO_CHANNEL  1

static volatile sig_atomic_t running = 1;
static void on_signal(int s) { (void)s; running = 0; }

int main(void) {
    /* TODO 1: Initialize logging, signals, and connect to larod. */
    /* TODO 2: Load the inference model and read model input metadata. */
    /* TODO 3: Allocate and mmap inference output tensors. */
    /* TODO 4: Create the VDO stream and compare it with model input requirements. */
    /* TODO 5: Configure cpu-proc preprocessing when format or size conversion is needed. */
    /* TODO 6: Create input tensors, run preprocessing/inference, and clean up. */

    return 0;
}
