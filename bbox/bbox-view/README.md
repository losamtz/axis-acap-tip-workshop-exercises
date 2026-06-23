# Bbox View Exercise

This exercise draws a static red bounding box on video channel 1 using the bbox API.

`app/bbox_view.c` keeps the GLib main loop, signal handling, cleanup, and error handling in place so the exercise can focus on the bbox drawing workflow.

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

## Step 3: Create the bbox view

Open `app/bbox_view.c`.

Paste this where the file says `TODO 1`:

```c
bbox_t* bbox = bbox_view_new(1u);
if (!bbox)
    panic("Failed creating: %s", strerror(errno));
```

This creates a bbox drawing context for video channel 1.

## Step 4: Clear old boxes

Paste this where the file says `TODO 2`:

```c
bbox_clear(bbox);
```

This removes previously drawn bounding boxes from the view.

## Step 5: Configure the box style

Paste this where the file says `TODO 3`:

```c
const bbox_color_t red = bbox_color_from_rgb(0xff, 0x00, 0x00);

bbox_style_outline(bbox);
bbox_thickness_thin(bbox);
bbox_color(bbox, red);
```

This configures a thin red outline for the bounding box.

## Step 6: Draw and commit the rectangle

Paste this where the file says `TODO 4`:

```c
bbox_rectangle(bbox, 0.05, 0.05, 0.95, 0.95);

if (!bbox_commit(bbox, 0u))
    panic("Failed committing: %s", strerror(errno));
```

This queues a rectangle and commits the queued geometry to the overlay.

## Build

From this example directory:

```sh
docker build --tag bbox-view --build-arg ARCH=aarch64 .
docker cp $(docker create bbox-view):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the application, start it, and follow the [test guide](.test/test.md).

## Reference

Complete source: `bbox/bbox-view` in `axis-acap-tip-workshop`.
