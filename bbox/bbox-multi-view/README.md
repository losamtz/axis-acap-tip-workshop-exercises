# Bbox Multi View Exercise

This exercise animates a yellow bounding box across views 1, 2, 3, and 4 using the bbox API.

`app/bbox_multi_view.c` keeps the GLib main loop, signal handling, cleanup, and timer setup in place so the exercise can focus on drawing one animation frame with bbox.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = bbox gio-2.0 glib-2.0
```

## Step 2: Add bbox access to manifest.json

Open `app/manifest.json`.

After `schemaVersion`, add the `resources` block below. Remember to add a comma after the `schemaVersion` line and keep the comma after the closing brace of `resources`.

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
    }
},
```

This gives the app access to the graphics and overlay D-Bus APIs and the `video` Linux group required by the bbox API.

## Step 3: Create a multi-view bbox handle

Open `app/bbox_multi_view.c`.

Paste this where the file says `TODO 1`:

```c
bbox_t* bbox = bbox_new(4u, 1u, 2u, 3u, 4u);
if (!bbox)
    panic("Failed creating: %s", strerror(errno));
```

This creates a bbox handle that targets views 1, 2, 3, and 4.

## Step 4: Enable output and clear the frame

Paste this where the file says `TODO 2`:

```c
if (!bbox_video_output(bbox, true))
    panic("Failed enabling video-output: %s", strerror(errno));

bbox_clear(bbox);
```

This enables overlay output and removes the previous frame's boxes.

## Step 5: Configure the box style

Paste this where the file says `TODO 3`:

```c
const bbox_color_t yellow = bbox_color_from_rgb(0xff, 0xff, 0x00);

bbox_style_corners(bbox);
bbox_thickness_medium(bbox);
bbox_color(bbox, yellow);
```

This configures a medium yellow corner-style box.

## Step 6: Update the animation position

Paste this where the file says `TODO 4`:

```c
xpos += dir * 0.02;

if (xpos + box_width >= 1.0) {
    xpos = 1.0 - box_width;
    dir = -1;
} else if (xpos <= 0.0) {
    xpos = 0.0;
    dir = 1;
}
```

This moves the box horizontally and changes direction at the edges.

## Step 7: Draw and commit the frame

Paste this where the file says `TODO 5`:

```c
bbox_rectangle(bbox, xpos, y, xpos + box_width, y + height);

if (!bbox_commit(bbox, 0u))
    panic("Failed committing: %s", strerror(errno));

sleep(1);
bbox_destroy(bbox);
```

This draws the current frame, commits it to all targeted views, waits briefly, and destroys the per-frame bbox handle.

## Build

From this example directory:

```sh
docker build --tag bbox-multi-view --build-arg ARCH=aarch64 .
docker cp $(docker create bbox-multi-view):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `bbox/bbox-multi-view` in `axis-acap-tip-workshop`.
