# Test Web Proxy Angular

Use this guide after building, installing, and starting the `web_proxy_angular` app.

## What to test

The app should serve the packaged Angular settings page and expose the reverse-proxy API:

- static UI from `app/html/index.html`
- `GET /local/web_proxy/api/info`
- `POST /local/web_proxy/api/param`

The Angular build must use `<base href="./">` so hashed JavaScript and CSS files load relative to the settings page.

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `Web Proxy Angular`.
3. Open the app settings page.
4. Confirm that the Angular UI renders, including styles and the Axis logo.
5. Confirm that the multicast address and port fields load.
6. Change the values and save.
7. Refresh the settings page and confirm that the saved values are still shown.

## Test with curl

Read current values:

```sh
curl --anyauth -u root:pass \
  "http://192.168.0.90/local/web_proxy/api/info"
```

Update values:

```sh
curl --anyauth -u root:pass \
  -H "Content-Type: application/json" \
  -d '{"MulticastAddress":"224.0.0.2","MulticastPort":"2048"}' \
  "http://192.168.0.90/local/web_proxy/api/param"
```

## Check frontend base href

Confirm the packaged page contains:

```html
<base href="./">
```

If scripts or styles fail to load, check `../acap-angular-ui/angular.json` and `../acap-angular-ui/src/index.html`.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=web_proxy_angular"
```

The log should show `CivetWeb listening on port 2001` and request URI messages for `/local/web_proxy/api/info` and `/local/web_proxy/api/param`.

## Troubleshooting

If the UI loads but API calls fail, confirm that:

- the manifest contains `reverseProxy` with `"apiPath": "api"`
- the reverse proxy target is `http://localhost:2001`
- the C handlers are registered for `/local/web_proxy/api/info` and `/local/web_proxy/api/param`
- the Angular API client uses `/local/web_proxy/api`
