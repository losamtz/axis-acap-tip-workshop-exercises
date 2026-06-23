# Parameter Custom Interface Exercise

This exercise connects application settings from a custom settings page to device system parameters. The callbacks are already present in `app/parameter_custom_interface.c` so the exercise can focus on configuration and the AXParameter setup flow.

The provided callbacks update:

- `root.Network.RTP.R0.VideoAddress`
- `root.Network.RTP.R0.VideoPort`

## Provided helper files

These files are kept complete:

- `app/panic.c`
- `app/panic.h`

## Step 1: Add manifest configuration

Open `app/manifest.json` and add this `configuration` object inside `acapPackageConf`, after the `setup` object:

```json
"configuration": {
  "settingPage": "index.html",
  "paramConfig": [
    {
      "name": "MulticastAddress",
      "default": "224.0.0.1",
      "type": "string:maxlen=64"
    },
    {
      "name": "MulticastPort",
      "default": "1024",
      "type": "int:maxlen=5;min=1024;max=65535"
    }
  ]
}
```

Keep the JSON comma between `setup` and `configuration` valid.

## Step 2: Add AXParameter to the Makefile

Open `app/Makefile` and add `axparameter` to `PKGS`:

```make
PKGS = glib-2.0 gio-2.0 axparameter
```

## Step 3: Create the AXParameter handle

Paste this at `TODO 3` in `main()`:

```c
axparameter = ax_parameter_new(app_name, &error);
if (!axparameter) {
    panic("ax_parameter_new failed: %s", error->message);
}
```

## Step 4: Register callbacks

Paste this at `TODO 4`:

```c
if (!ax_parameter_register_callback(axparameter,
                                    "MulticastAddress",
                                    multicast_address_callback,
                                    NULL,
                                    &error)) {
    panic("register MulticastAddress failed: %s", error->message);
}

if (!ax_parameter_register_callback(axparameter,
                                    "MulticastPort",
                                    multicast_port_callback,
                                    NULL,
                                    &error)) {
    panic("register MulticastPort failed: %s", error->message);
}
```

## Step 5: Run the GLib loop and clean up

Paste this at `TODO 5`:

```c
main_loop = g_main_loop_new(NULL, FALSE);
g_unix_signal_add(SIGTERM, signal_handler, main_loop);
g_unix_signal_add(SIGINT, signal_handler, main_loop);
g_main_loop_run(main_loop);

g_main_loop_unref(main_loop);
ax_parameter_free(axparameter);
```

## Build

From this example directory:

```sh
docker build --tag parameter-custom-interface --build-arg ARCH=aarch64 .
docker cp $(docker create parameter-custom-interface):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the application, open the app settings page, change the multicast address or port, and inspect the application log. The callbacks should write the new values to the corresponding system parameters.
