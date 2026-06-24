# Test Web Proxy

Use this guide after building, installing, and starting the `web_proxy` app.

## What to test

The app should run a local CivetWeb server on port `2001`, while the camera reverse proxy exposes:

- `GET /local/web_proxy/api/info`
- `POST /local/web_proxy/api/param`

The bundled settings page should use those endpoints to read and save `MulticastAddress` and `MulticastPort`.

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `Web Proxy`.
3. Open the app settings page.
4. Confirm that the multicast address and port fields load.
5. Change the values and save.
6. Refresh the settings page and confirm that the saved values are still shown.

## Test with curl

Read current values:

```sh
curl --anyauth -u root:pass \
  "http://192.168.0.90/local/web_proxy/api/info"
```

Expected response shape:

```json
{"MulticastAddress":"224.0.0.1","MulticastPort":"1024","ok":true}
```

Update values:

```sh
curl --anyauth -u root:pass \
  -H "Content-Type: application/json" \
  -d '{"MulticastAddress":"224.0.0.2","MulticastPort":"2048"}' \
  "http://192.168.0.90/local/web_proxy/api/param"
```

Expected response shape:

```json
{"ok":true,"changed":true}
```

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=web_proxy"
```

The log should show server startup and request URI messages such as:

```text
CivetWeb listening on port 2001
URI: /local/web_proxy/api/info  method: GET
URI: /local/web_proxy/api/param  method: POST
```

## Troubleshooting

If the settings page loads but the API does not respond, confirm that:

- the app is running
- the manifest contains the `resources` block from the README
- the manifest contains `settingPage`, `reverseProxy`, and `paramConfig`
- the reverse proxy target is `http://localhost:2001`
- the Makefile links `jansson`, `axparameter`, and CivetWeb
- the registered handler paths match `/local/web_proxy/api/info` and `/local/web_proxy/api/param`
