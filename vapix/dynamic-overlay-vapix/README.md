# Dynamic Overlay Vapix Exercise

This exercise is based on `vapix/dynamic-overlay-vapix` from the complete `axis-acap-tip-workshop` repository.

`app/dynamic_overlay_vapix.c` keeps the main VAPIX workflow in place so the exercise can focus on configuring the build and manifest access needed by the curl request and VAPIX credentials helper.

## Step 1: Add build dependencies

Open `app/Makefile` and replace the TODO `PKGS` line with:

```make
PKGS = gio-2.0 libcurl jansson
```

## Step 2: Add VAPIX credentials access to manifest.json

Open `app/manifest.json`.

After `schemaVersion`, add the `resources` block below. Remember to add a comma after the `schemaVersion` line and keep the comma after the closing brace of `resources`.

```json
"resources": {
    "dbus": {
        "requiredMethods": ["com.axis.HTTPConf1.VAPIXServiceAccounts1.GetCredentials"]
    }
},
```

This allows the app to request VAPIX service account credentials over D-Bus.

## Step 3: Initialize curl

Open `app/dynamic_overlay_vapix.c`.

Paste this where the file says `TODO 1`:

```c
curl_global_init(CURL_GLOBAL_DEFAULT);
CURL* handle = curl_easy_init();
```

This initializes libcurl and creates the curl handle used by the VAPIX request helper.

## Step 4: Retrieve VAPIX credentials

Open `app/vapix-credentials.c`.

Paste this where the file says `TODO 2`:

```c
GError* error               = NULL;
GDBusConnection* connection = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
if (!connection)
    panic("Error connecting to D-Bus: %s", error->message);

const char* bus_name       = "com.axis.HTTPConf1";
const char* object_path    = "/com/axis/HTTPConf1/VAPIXServiceAccounts1";
const char* interface_name = "com.axis.HTTPConf1.VAPIXServiceAccounts1";
const char* method_name    = "GetCredentials";

GVariant* result = g_dbus_connection_call_sync(connection,
                                               bus_name,
                                               object_path,
                                               interface_name,
                                               method_name,
                                               g_variant_new("(s)", username),
                                               NULL,
                                               G_DBUS_CALL_FLAGS_NONE,
                                               -1,
                                               NULL,
                                               &error);
if (!result)
    panic("Error invoking D-Bus method: %s", error->message);

char* credentials = parse_credentials(result);

g_variant_unref(result);
g_object_unref(connection);
return credentials;
```

This calls `GetCredentials` for the VAPIX service account user and returns a `username:password` string for curl.

## Step 5: Send the VAPIX POST request

Open `app/curl-request.c`.

Paste this where the file says `TODO 3`:

```c
GString* response = g_string_new(NULL);
char* url         = g_strdup_printf("http://127.0.0.12/%s", endpoint);

curl_easy_setopt(handle, CURLOPT_URL, url);
curl_easy_setopt(handle, CURLOPT_USERPWD, credentials);
curl_easy_setopt(handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
curl_easy_setopt(handle, CURLOPT_POSTFIELDS, request);
curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, append_to_gstring_callback);
curl_easy_setopt(handle, CURLOPT_WRITEDATA, response);

CURLcode res = curl_easy_perform(handle);
if (res != CURLE_OK)
    panic("curl_easy_perform error %d: '%s'", res, curl_easy_strerror(res));

long response_code;
curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response_code);
if (response_code != 200)
    panic("Got response code %ld from request to %s with response '%s'",
          response_code,
          request,
          response->str);

free(url);
return g_string_free(response, FALSE);
```

This builds the local VAPIX URL, configures curl authentication and request data, performs the POST, and returns the response body.

## Step 6: Build the addText JSON request

Open `app/dynamic_overlay_vapix.c`.

Paste this where the file says `TODO 4`:

```c
static json_t* build_addtext_request(void) {
    json_t* root = json_object();
    json_t* params = json_object();

    // Fill the "params" object
    json_object_set_new(params, "camera", json_integer(1));
    json_object_set_new(params, "text", json_string("AXIS TIP Paris workshop - Date: %c"));
    json_object_set_new(params, "position", json_string("topLeft"));
    json_object_set_new(params, "textColor", json_string("white"));
    json_object_set_new(params, "fontSize", json_integer(60));
    json_object_set_new(params, "textBGColor", json_string("black"));

    // Fill the root object
    json_object_set_new(root, "apiVersion", json_string("1.0"));
    json_object_set_new(root, "context", json_string("abc"));
    json_object_set_new(root, "method", json_string("addText"));
    json_object_set_new(root, "params", params);

    return root;
}
```

This creates the JSON-RPC `addText` request body for the dynamic overlay VAPIX API.

## Step 7: Add dynamic overlay text

Open `app/dynamic_overlay_vapix.c`.

Paste this where the file says `TODO 5`:

```c
static json_t* add_text(CURL* handle, const char* credentials) {
    const char* endpoint = "/axis-cgi/dynamicoverlay/dynamicoverlay.cgi";

    json_t* request_obj = build_addtext_request();
    char* request = json_dumps(request_obj, JSON_COMPACT);

    return vapix_post_json(handle, credentials, endpoint, request);
}
```

This creates the `addText` JSON request and sends it to the dynamic overlay VAPIX endpoint.

## Build

From this example directory:

```sh
docker build --tag dynamic-overlay-vapix --build-arg ARCH=aarch64 .
docker cp $(docker create dynamic-overlay-vapix):/opt/app ./build
```

The generated `.eap` package will be copied into `./build`.

## Verify

Install the `.eap` on a camera and verify the behavior described by the exercise code and comments. Use the application log to confirm the main API calls run in the expected order.

## Reference

Complete source: `vapix/dynamic-overlay-vapix` in `axis-acap-tip-workshop`.
