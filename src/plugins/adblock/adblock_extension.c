/*
 * Rosenrot Adblock Web Extension
 * 
 * This is a WebKit web extension that runs in the web process and
 * intercepts HTTP requests to block ads using easylist.txt filters.
 *
 * Copyright 2024 Rosenrot contributors
 * License: GPL-3.0
 */

#include <webkit/webkit-web-process-extension.h>
#include "ephy-uri-tester.h"

/* Configuration - must match ADBLOCK_FILTERLIST_PATH in config.h */
#ifndef ADBLOCK_FILTERLIST_PATH
#define ADBLOCK_FILTERLIST_PATH "/opt/rosenrot/easylist.txt"
#endif

static EphyUriTester* tester = NULL;
static gboolean adblock_enabled = TRUE;

/*
 * Called for each resource request (images, scripts, stylesheets, etc.)
 * Returns TRUE to block the request, FALSE to allow it.
 */
static gboolean
on_send_request(WebKitWebPage* page,
    WebKitURIRequest* request,
    WebKitURIResponse* redirect_response G_GNUC_UNUSED,
    gpointer user_data G_GNUC_UNUSED)
{
    if (!adblock_enabled || !tester)
        return FALSE;

    const char* request_uri = webkit_uri_request_get_uri(request);
    const char* page_uri = webkit_web_page_get_uri(page);

    /* Don't block the main page itself */
    if (g_strcmp0(request_uri, page_uri) == 0)
        return FALSE;

    /* Skip non-HTTP(S) requests */
    if (!g_str_has_prefix(request_uri, "http://") &&
        !g_str_has_prefix(request_uri, "https://"))
        return FALSE;

    /* Check if this URI should be blocked */
    if (ephy_uri_tester_test_uri(tester, request_uri, page_uri)) {
        g_debug("[rosenrot-adblock] Blocked: %s", request_uri);
        return TRUE; /* Block the request */
    }

    return FALSE; /* Allow the request */
}

/*
 * Called when a new web page is created.
 * We connect to the send-request signal to intercept all resource requests.
 */
static void
on_page_created(WebKitWebProcessExtension* extension,
    WebKitWebPage* page,
    gpointer user_data)
{
    (void)extension;
    (void)user_data;
    g_signal_connect(page, "send-request", G_CALLBACK(on_send_request), NULL);
}

/*
 * Extension initialization - called by WebKit when loading the extension.
 * 
 * The user_data variant can contain configuration from the main browser process:
 * - "enabled": gboolean to enable/disable adblock
 */
G_MODULE_EXPORT void
webkit_web_process_extension_initialize_with_user_data(WebKitWebProcessExtension* extension,
    GVariant* user_data)
{
    /* Parse configuration from user data */
    if (user_data && g_variant_is_of_type(user_data, G_VARIANT_TYPE_VARDICT)) {
        GVariant* enabled_var = g_variant_lookup_value(user_data, "enabled", G_VARIANT_TYPE_BOOLEAN);
        if (enabled_var) {
            adblock_enabled = g_variant_get_boolean(enabled_var);
            g_variant_unref(enabled_var);
        }
    }

    if (!adblock_enabled) {
        g_debug("[rosenrot-adblock] Adblock disabled by configuration");
        return;
    }

    /* Check if filter file exists */
    if (!g_file_test(ADBLOCK_FILTERLIST_PATH, G_FILE_TEST_EXISTS)) {
        g_warning("[rosenrot-adblock] Filter file not found: %s", ADBLOCK_FILTERLIST_PATH);
        g_warning("[rosenrot-adblock] Download easylist.txt from https://easylist.to/easylist/easylist.txt");
        return;
    }

    /* Initialize the URI tester and load filters */
    tester = ephy_uri_tester_new(ADBLOCK_FILTERLIST_PATH);
    ephy_uri_tester_load(tester);

    g_debug("[rosenrot-adblock] Loaded filters from %s", ADBLOCK_FILTERLIST_PATH);

    /* Connect to page-created to hook into each new page */
    g_signal_connect(extension, "page-created", G_CALLBACK(on_page_created), NULL);
}

/*
 * Fallback initialization without user data
 */
G_MODULE_EXPORT void
webkit_web_process_extension_initialize(WebKitWebProcessExtension* extension)
{
    webkit_web_process_extension_initialize_with_user_data(extension, NULL);
}
