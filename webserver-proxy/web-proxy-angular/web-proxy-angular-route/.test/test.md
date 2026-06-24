# Test Web Proxy Angular Route

Use this guide after building, installing, and starting the `web_proxy_angular_route` app.

## What to test

The app should serve the packaged routed Angular settings page and expose the same reverse-proxy API:

- static UI from `app/html/index.html`
- frontend routes resolved through `index.html`
- `GET /local/web_proxy/api/info`
- `POST /local/web_proxy/api/param`

The Angular build must use `<base href="./index.html">` so client-side routes resolve from the packaged settings page.

## Test from the camera web UI

1. Open `http://192.168.0.90/camera/index.html#/apps`.
2. Install and start `Web Proxy Angular Route`.
3. Open the app settings page.
4. Confirm that the Angular UI renders, including styles and the Axis logo.
5. Navigate between the Angular routes in the UI.
6. Confirm that API-backed fields still load and save.
7. Refresh the settings page and confirm that the routed UI still loads.

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
<base href="./index.html">
```

If refresh or route navigation fails, check `../acap-angular-ui-routing/angular.json` and `../acap-angular-ui-routing/src/index.html`.

## Check logs

Open the application logs or use:

```sh
curl --anyauth -u root:pass "http://192.168.0.90/axis-cgi/admin/systemlog.cgi?appname=web_proxy_angular_route"
```

The log should show `CivetWeb listening on port 2001` and request URI messages for `/local/web_proxy/api/info` and `/local/web_proxy/api/param`.

## Troubleshooting

If routes work but API calls fail, confirm that:

- the manifest contains `reverseProxy` with `"apiPath": "api"`
- the reverse proxy target is `http://localhost:2001`
- the C handlers are registered for `/local/web_proxy/api/info` and `/local/web_proxy/api/param`
- the Angular API client still uses `/local/web_proxy/api`
