# Test Web Parameter

Use this guide after building, installing, and starting the `web_parameter` app.

## What to test

The app should expose two admin-only FastCGI endpoints:

- `GET /local/web_parameter/info-acap.cgi` returns current parameter values as JSON
- `POST /local/web_parameter/param-acap.cgi` updates parameter values from a JSON body

The bundled settings page should use those endpoints to read and save `MulticastAddress` and `MulticastPort`.

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `Web Parameter`.
3. Open the app settings page.
4. Confirm that the multicast address and port fields load.
5. Change the values and save.
6. Refresh the settings page and confirm that the saved values are still shown.

## Test with curl

Read current values:

```sh
curl --anyauth -u root:pass \
  "http://192.168.0.90/local/web_parameter/info-acap.cgi"
```

Expected response shape:

```json
{"ok":true,"MulticastAddress":"224.0.0.1","MulticastPort":"1024"}
```

Update values:

```sh
curl --anyauth -u root:pass \
  -H "Content-Type: application/json" \
  -d '{"MulticastAddress":"224.0.0.2","MulticastPort":"2048"}' \
  "http://192.168.0.90/local/web_parameter/param-acap.cgi"
```

Expected response shape:

```json
{"ok":true,"MulticastAddress":"224.0.0.2","MulticastPort":"2048","changed":true}
```

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=web_parameter"
```

The log should show startup, the FastCGI socket path, accepted requests, and route messages such as:

```text
FCGI GET /local/web_parameter/info-acap.cgi
FCGI POST /local/web_parameter/param-acap.cgi
```

## Troubleshooting

If the endpoints return 404 or the settings page does not load values, confirm that:

- the app is running
- the manifest contains the `resources` block from the README
- the manifest contains `settingPage`, `httpConfig`, and `paramConfig`
- the Makefile links `fcgi`, `axparameter`, and `jansson`
- the `SCRIPT_NAME` strings in `route_request()` match the endpoint paths
- the app log shows a valid `FCGI_SOCKET_NAME`
