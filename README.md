# Axis ACAP TIP Workshop Exercises

This repository is an exercise version of `axis-acap-tip-workshop`.
It keeps the same workshop folder structure, Dockerfiles, manifests, Makefiles, assets, and helper files, but the primary C file in each buildable example has been reduced to a TODO skeleton.

The corresponding `README.md` in each example contains:

- what file to edit
- the implementation snippet to paste into the C file
- build commands
- a short verification note

The original `axis-acap-tip-workshop` repository remains the complete reference. Try the exercises first, then compare with the original workshop if you get stuck.

## Common Build Flow

From any example directory containing a `Dockerfile`:

```sh
docker build --tag <example-name> --build-arg ARCH=aarch64 .
docker cp $(docker create <example-name>):/opt/app ./build
```

The generated `.eap` package will be in `./build`.

## Learning Order

1. `axis-intro/minimal-app`
2. `parameter/`
3. `vapix/`
4. `event/`
5. `overlay/` and `overlay2/`
6. `bbox/`
7. `vdo/`
8. `larod/`
9. `webserver-fastcgi/`
10. `webserver-proxy/`

## Exercise Index

- `axis-intro/minimal-app`: edit `axis_intro_minimal.c`
- `bbox/bbox-multi-view`: edit `bbox_multi_view.c`
- `bbox/bbox-multi-view-refactor-lab`: edit `bbox_multi_view_lab.c`
- `bbox/bbox-view`: edit `bbox_view.c`
- `event/send-events-types/send-data`: edit `send_data.c`
- `event/send-events-types/send-pulse`: edit `send_pulse.c`
- `event/send-events-types/send-pulse-dropdown`: edit `send_pulse_drop_down.c`
- `event/send-events-types/send-state`: edit `send_state.c`
- `event/send-events-types/send-state-with-data`: edit `send_state_with_data.c`
- `event/subcribe-event-data`: edit `subscribe_event_data.c`
- `larod/larod-basic`: edit `larod_basic.c`
- `larod/larod-client`: edit `larod_client_tool.c`
- `larod/larod-preprocessing`: edit `larod_preprocessing.c`
- `larod/object-detection-min`: edit `object_detection_min.c`
- `larod/vdo-larod-min`: edit `vdo_larod_min.c`
- `overlay/add-logo`: edit `add_logo.c`
- `overlay/draw-rectangle`: edit `draw_rectangle.c`
- `overlay/draw-text`: edit `draw_text.c`
- `overlay/draw-views`: edit `draw_views.c`
- `overlay/dynamic-overlay-vapix`: edit `dynamic_overview_vapix.c`
- `overlay2/add-logo`: edit `overlay2_add_logo.c`
- `overlay2/draw-rectangle`: edit `overlay2_draw_rectangle.c`
- `overlay2/draw-text`: edit `overlay2_draw_text.c`
- `overlay2/draw-views`: edit `overlay2_draw_views.c`
- `parameter/parameter-custom-interface`: edit `parameter_custom_interface.c`
- `parameter/parameter-manifest`: edit `parameter_manifest.c`
- `parameter/parameter-runtime`: edit `parameter_runtime.c`
- `vapix/dynamic-overlay-vapix`: edit `dynamic_overlay_vapix.c`
- `vapix/onvif-request`: edit `onvif_request.c`
- `vdo/vdo-dma-bufs`: edit `vdo_dma_bufs.c`
- `vdo/vdo-stream-blocking`: edit `vdo_stream_blocking.c`
- `vdo/vdo-stream-events`: edit `vdo_stream_events.c`
- `vdo/vdo-stream-non-block`: edit `vdo_stream_non_block.c`
- `vdo/vdo-stream-nv12`: edit `vdo_stream_nv12.c`
- `vdo/vdo-stream-rgb`: edit `vdo_stream_rgb.c`
- `vdo/vdo-utilities`: edit `vdo_utilities.c`
- `webserver-fastcgi/web-parameter`: edit `web_parameter.c`
- `webserver-fastcgi/web-parameter-thread`: edit `web_parameter_thread.c`
- `webserver-proxy/web-proxy`: edit `web_proxy.c`
- `webserver-proxy/web-proxy-angular/web-proxy-angular`: edit `web_proxy_angular.c`
- `webserver-proxy/web-proxy-angular/web-proxy-angular-route`: edit `web_proxy_angular_route.c`
- `webserver-proxy/web-proxy-thread`: edit `web_proxy_thread.c`

## Notes for Instructors

This repository intentionally does not include a `solutions/` folder. The complete workshop repository is the solution reference.
