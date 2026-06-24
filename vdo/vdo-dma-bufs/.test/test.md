# Test VDO DMA-BUFs

Use this guide after building, installing, and starting the `vdo_dma_bufs` app.

## What to test

The app should create a non-blocking YUV VDO stream, log stream info, poll for frames, inspect fd-backed VDO buffers, map a bounded byte range with `mmap()`, return each buffer, and keep running until stopped.

Expected log content includes:

- stream width, height, pitch, format, buffer type, and buffer count
- DMA-BUF fd, offset, capacity, and frame size
- a bounded hex dump of the first bytes at image offset
- a shutdown message after the app is stopped

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `vdo dma bufs`.
3. Let it run for a few seconds.
4. Stop the app.
5. Open the application log.
6. Confirm that stream info and multiple DMA-BUF inspection messages were logged.

## Check logs

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=vdo_dma_bufs"
```

## Troubleshooting

If no DMA-BUF details are logged, confirm that:

- the manifest contains the `resources` block from the README
- the Makefile links `vdostream`
- video channel 1 exists
- the stream info shows an fd-backed buffer type
- `vdo_stream_buffer_unref()` is called after inspection
- no expected maintenance or rotation-related VDO error is reported
