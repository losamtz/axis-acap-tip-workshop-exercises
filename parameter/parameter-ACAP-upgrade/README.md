# Parameter ACAP Upgrade Exercise

This exercise demonstrates configuration persistence across ACAP application upgrades using two app-owned settings:

- `ACAPName`, stored through AXParameter and displayed as the page heading.
- `backgroundColour`, stored in a local JSON file and applied to the page background.

Neither setting changes device-wide parameters. The FastCGI settings page is available to administrators.

The exercise's eventual upgrade workflow is:

1. Build and install version 1.0.0.
2. Change configuration through the app and verify the values.
3. Build version 2.0.0 and upgrade the same installation.
4. Verify the changed configuration was preserved.

## Application files

- `app/parameter_ACAP_upgrade.c` contains the AXParameter callbacks and FastCGI settings page.
- `app/panic.c` and `app/panic.h` provide fatal-error logging.
- `app/manifest.json` declares the app parameters and admin-only FastCGI route.

The settings page is served directly by FastCGI at:

```text
http://<DEVICE-IP>/local/parameter_ACAP_upgrade/settings.cgi
```

Saving the form updates the displayed ACAP name and page background immediately. The JSON file is stored at `/usr/local/packages/parameter_ACAP_upgrade/localdata/configuration.json`.

## Build

In your WSL terminal, change to this example directory: the folder containing this `README.md`, `Dockerfile`, and `app/`. The absolute path depends on where you cloned the repository, but `pwd` should end with:

```text
parameter/parameter-ACAP-upgrade
```

Run the build commands from there:

```sh
docker build --tag parameter-acap-upgrade --build-arg ARCH=aarch64 .
docker cp "$(docker create parameter-acap-upgrade)":/opt/app/. ./build/
```

The generated ACAP package is copied into `./build`.

## Verify

Install and start the app, then open the FastCGI settings page:

```text
http://<DEVICE-IP>/local/parameter_ACAP_upgrade/settings.cgi
```

Confirm the heading shows the current `ACAPName` and the page background uses the saved `backgroundColour`. Change both values and select **Save settings**. Confirm the page updates immediately, then refresh and verify both values persist.

`ACAPName` is stored through AXParameter. `backgroundColour` is stored as JSON in the app's persistent local data directory:

```text
/usr/local/packages/parameter_ACAP_upgrade/localdata/configuration.json
```

### Test an external `ACAPName` change

Keep the settings page open and change `ACAPName` outside the app, for example with VAPIX:

```sh
curl --anyauth -u root:pass "http://<DEVICE-IP>/axis-cgi/param.cgi?action=update&root.Parameter_ACAP_upgrade.ACAPName=Intruder"
```

Within a few seconds, the page heading should show `Intruder`, and the **Parameter log** should show the old and new values. After 10 seconds, the app should restore its own name and log `I am the master of my parameters here!`.

### Verify upgrade persistence

After confirming both settings, change the app version in the manifest from `1.0.0` to `2.0.0`, build and upgrade the same installation. Reopen the page and verify the name and background colour were preserved.
