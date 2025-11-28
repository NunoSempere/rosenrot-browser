/*
 *  Copyright © 2011 Igalia S.L.
 *
 *  This file is part of Epiphany.
 *
 *  Epiphany is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Epiphany is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Epiphany.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Some parts based on Midori's 'adblock' extension.
 *  Adapted for rosenrot browser.
 */

#include "uri_tester.h"
#include <string.h>

#define SIGNATURE_SIZE 8

struct _AdblockUriTester {
    GObject parent_instance;

    char* filter_file_path;

    GHashTable* pattern;
    GHashTable* keys;
    GHashTable* optslist;
    GHashTable* urlcache;

    GHashTable* whitelisted_pattern;
    GHashTable* whitelisted_keys;
    GHashTable* whitelisted_optslist;
    GHashTable* whitelisted_urlcache;

    GRegex* regex_third_party;
    GRegex* regex_pattern;
    GRegex* regex_subdocument;

    gboolean loaded;
};

enum {
    PROP_0,
    PROP_FILTER_FILE_PATH,
    LAST_PROP
};

static GParamSpec* obj_properties[LAST_PROP];

G_DEFINE_TYPE(AdblockUriTester, adblock_uri_tester, G_TYPE_OBJECT)

static GString* fixup_regexp(const char* prefix, char* src);

static inline gboolean
check_rule(AdblockUriTester* tester,
    GRegex* regex,
    const char* patt,
    const char* req_uri,
    const char* page_uri,
    gboolean whitelist)
{
    char* opts;
    GHashTable* optslist = whitelist ? tester->whitelisted_optslist : tester->optslist;

    if (!g_regex_match_full(regex, req_uri, -1, 0, 0, NULL, NULL))
        return FALSE;

    opts = g_hash_table_lookup(optslist, patt);
    if (opts && g_regex_match(tester->regex_third_party, opts, 0, NULL)) {
        if (page_uri && g_regex_match_full(regex, page_uri, -1, 0, 0, NULL, NULL))
            return FALSE;
    }
    return TRUE;
}

static inline gboolean
is_matched_by_pattern(AdblockUriTester* tester,
    const char* req_uri,
    const char* page_uri,
    gboolean whitelist)
{
    GHashTableIter iter;
    gpointer patt, regex;
    GHashTable* pattern = whitelist ? tester->whitelisted_pattern : tester->pattern;

    g_hash_table_iter_init(&iter, pattern);
    while (g_hash_table_iter_next(&iter, &patt, &regex)) {
        if (check_rule(tester, regex, patt, req_uri, page_uri, whitelist))
            return TRUE;
    }
    return FALSE;
}

static inline gboolean
is_matched_by_key(AdblockUriTester* tester,
    const char* req_uri,
    const char* page_uri,
    gboolean whitelist)
{
    int len;
    int pos = 0;
    GList* regex_bl = NULL;
    GString* guri;
    gboolean ret = FALSE;
    char sig[SIGNATURE_SIZE + 1];
    GHashTable* keys = whitelist ? tester->whitelisted_keys : tester->keys;

    memset(&sig[0], 0, sizeof(sig));
    guri = fixup_regexp("", (char*)req_uri);
    if (!guri)
        return FALSE;
    len = guri->len;

    for (pos = len - SIGNATURE_SIZE; pos >= 0; pos--) {
        GRegex* regex;
        strncpy(sig, guri->str + pos, SIGNATURE_SIZE);
        regex = g_hash_table_lookup(keys, sig);

        if (!regex || g_list_find(regex_bl, regex))
            continue;
        ret = check_rule(tester, regex, sig, req_uri, page_uri, whitelist);
        if (ret)
            break;
        regex_bl = g_list_prepend(regex_bl, regex);
    }
    g_string_free(guri, TRUE);
    g_list_free(regex_bl);
    return ret;
}

static gboolean
is_matched(AdblockUriTester* tester,
    const char* req_uri,
    const char* page_uri,
    gboolean whitelist)
{
    gpointer is_matched_val;
    GHashTable* urlcache = whitelist ? tester->whitelisted_urlcache : tester->urlcache;

    if (g_hash_table_lookup_extended(urlcache, req_uri, NULL, &is_matched_val))
        return GPOINTER_TO_INT(is_matched_val);

    if (is_matched_by_key(tester, req_uri, page_uri, whitelist)) {
        g_hash_table_insert(urlcache, g_strdup(req_uri), GINT_TO_POINTER(TRUE));
        return TRUE;
    }

    if (is_matched_by_pattern(tester, req_uri, page_uri, whitelist)) {
        g_hash_table_insert(urlcache, g_strdup(req_uri), GINT_TO_POINTER(TRUE));
        return TRUE;
    }

    g_hash_table_insert(urlcache, g_strdup(req_uri), GINT_TO_POINTER(FALSE));
    return FALSE;
}

static GString*
fixup_regexp(const char* prefix, char* src)
{
    GString* str;

    if (!src)
        return NULL;

    str = g_string_new(prefix);

    if (src[0] == '*') {
        src++;
    }

    do {
        switch (*src) {
        case '*':
            g_string_append(str, ".*");
            break;
        case '^':
            g_string_append(str, "([^a-zA-Z\\d]|[_\\-\\.%])");
            break;
        case '|':
            if (src[1] == '\0')
                g_string_append(str, "$");
            else
                g_string_append(str, "\\|");
            break;
        case '.':
        case '+':
        case '?':
        case '[':
        case ']':
        case '{':
        case '}':
        case '(':
        case ')':
        case '\\':
            g_string_append_printf(str, "\\%c", *src);
            break;
        default:
            g_string_append_printf(str, "%c", *src);
            break;
        }
        src++;
    } while (*src);

    return str;
}

static void
compile_regexp(AdblockUriTester* tester,
    GString* gpatt,
    const char* opts,
    gboolean whitelist)
{
    GHashTable* pattern;
    GHashTable* keys;
    GHashTable* optslist;
    GRegex* regex;
    GError* error = NULL;
    char* patt;
    int len;

    if (!gpatt)
        return;

    patt = gpatt->str;
    len = gpatt->len;

    regex = g_regex_new(patt, G_REGEX_OPTIMIZE, G_REGEX_MATCH_NOTEMPTY, &error);
    if (error) {
        g_error_free(error);
        return;
    }

    pattern = whitelist ? tester->whitelisted_pattern : tester->pattern;
    keys = whitelist ? tester->whitelisted_keys : tester->keys;
    optslist = whitelist ? tester->whitelisted_optslist : tester->optslist;

    if (!g_regex_match(tester->regex_pattern, patt, 0, NULL)) {
        int signature_count = 0;
        int pos = 0;
        char* sig;

        for (pos = len - SIGNATURE_SIZE; pos >= 0; pos--) {
            sig = g_strndup(patt + pos, SIGNATURE_SIZE);
            if (!strchr(sig, '*') && !g_hash_table_lookup(keys, sig)) {
                g_hash_table_insert(keys, g_strdup(sig), g_regex_ref(regex));
                g_hash_table_insert(optslist, g_strdup(sig), g_strdup(opts));
                signature_count++;
            } else {
                if (sig[0] == '*' && !g_hash_table_lookup(pattern, patt)) {
                    g_hash_table_insert(pattern, g_strdup(patt), g_regex_ref(regex));
                    g_hash_table_insert(optslist, g_strdup(patt), g_strdup(opts));
                }
            }
            g_free(sig);
        }
        g_regex_unref(regex);

        if (signature_count > 1 && g_hash_table_lookup(pattern, patt))
            g_hash_table_remove(pattern, patt);
    } else {
        g_hash_table_insert(pattern, g_strdup(patt), regex);
        g_hash_table_insert(optslist, g_strdup(patt), g_strdup(opts));
    }
}

static void
add_url_pattern(AdblockUriTester* tester,
    const char* prefix,
    const char* type,
    char* line,
    gboolean whitelist)
{
    char** data;
    char* patt;
    GString* format_patt;
    const char* opts;

    data = g_strsplit(line, "$", -1);
    if (!data || !data[0]) {
        g_strfreev(data);
        return;
    }

    if (data[1] && data[2]) {
        patt = g_strconcat(data[0], data[1], NULL);
        opts = g_strconcat(type, ",", data[2], NULL);
    } else if (data[1]) {
        patt = data[0];
        opts = g_strconcat(type, ",", data[1], NULL);
    } else {
        patt = data[0];
        opts = type;
    }

    if (g_regex_match(tester->regex_subdocument, opts, 0, NULL)) {
        if (data[1] && data[2])
            g_free(patt);
        if (data[1])
            g_free((char*)opts);
        g_strfreev(data);
        return;
    }

    format_patt = fixup_regexp(prefix, patt);

    compile_regexp(tester, format_patt, opts, whitelist);

    if (data[1] && data[2])
        g_free(patt);
    if (data[1])
        g_free((char*)opts);
    g_strfreev(data);

    if (format_patt)
        g_string_free(format_patt, TRUE);
}

static void
parse_line(AdblockUriTester* tester,
    char* line,
    gboolean whitelist)
{
    if (!line)
        return;

    g_strchomp(line);

    if (line[0] == '!')
        return;
    if (line[0] == '[')
        return;

    if (g_str_has_prefix(line, "@@")) {
        parse_line(tester, line + 2, TRUE);
        return;
    }

    if (strstr(line, "domain="))
        return;

    if (line[0] == ' ' || !line[0])
        return;

    if (line[0] == '#')
        return;

    if (strstr(line, "##"))
        return;

    if (strchr(line, '#'))
        return;

    if (line[0] == '|' && line[1] == '|') {
        line += 2;
        add_url_pattern(tester, "^[\\w\\-]+:\\/+(?!\\/)(?:[^\\/]+\\.)?", "fulluri", line, whitelist);
        return;
    }
    if (line[0] == '|') {
        line++;
        add_url_pattern(tester, "^", "fulluri", line, whitelist);
        return;
    }
    add_url_pattern(tester, "", "uri", line, whitelist);
}

static void
load_filters_sync(AdblockUriTester* tester)
{
    GFile* file;
    GFileInputStream* stream;
    GDataInputStream* data_stream;
    GError* error = NULL;
    char* line;

    if (!tester->filter_file_path)
        return;

    file = g_file_new_for_path(tester->filter_file_path);
    if (!g_file_query_exists(file, NULL)) {
        g_object_unref(file);
        return;
    }

    stream = g_file_read(file, NULL, &error);
    g_object_unref(file);

    if (!stream) {
        if (error) {
            g_warning("Failed to open filter file: %s", error->message);
            g_error_free(error);
        }
        return;
    }

    data_stream = g_data_input_stream_new(G_INPUT_STREAM(stream));
    g_object_unref(stream);

    while ((line = g_data_input_stream_read_line(data_stream, NULL, NULL, NULL)) != NULL) {
        parse_line(tester, line, FALSE);
        g_free(line);
    }

    g_object_unref(data_stream);
    tester->loaded = TRUE;
}

static void
adblock_uri_tester_init(AdblockUriTester* tester)
{
    tester->pattern = g_hash_table_new_full(g_str_hash, g_str_equal,
        (GDestroyNotify)g_free, (GDestroyNotify)g_regex_unref);
    tester->keys = g_hash_table_new_full(g_str_hash, g_str_equal,
        (GDestroyNotify)g_free, (GDestroyNotify)g_regex_unref);
    tester->optslist = g_hash_table_new_full(g_str_hash, g_str_equal,
        (GDestroyNotify)g_free, (GDestroyNotify)g_free);
    tester->urlcache = g_hash_table_new_full(g_str_hash, g_str_equal,
        (GDestroyNotify)g_free, NULL);

    tester->whitelisted_pattern = g_hash_table_new_full(g_str_hash, g_str_equal,
        (GDestroyNotify)g_free, (GDestroyNotify)g_regex_unref);
    tester->whitelisted_keys = g_hash_table_new_full(g_str_hash, g_str_equal,
        (GDestroyNotify)g_free, (GDestroyNotify)g_regex_unref);
    tester->whitelisted_optslist = g_hash_table_new_full(g_str_hash, g_str_equal,
        (GDestroyNotify)g_free, (GDestroyNotify)g_free);
    tester->whitelisted_urlcache = g_hash_table_new_full(g_str_hash, g_str_equal,
        (GDestroyNotify)g_free, NULL);

    tester->regex_third_party = g_regex_new(",third-party",
        G_REGEX_CASELESS | G_REGEX_OPTIMIZE, G_REGEX_MATCH_NOTEMPTY, NULL);
    tester->regex_pattern = g_regex_new("^/.*[\\^\\$\\*].*/$",
        G_REGEX_UNGREEDY | G_REGEX_OPTIMIZE, G_REGEX_MATCH_NOTEMPTY, NULL);
    tester->regex_subdocument = g_regex_new("subdocument",
        G_REGEX_CASELESS | G_REGEX_OPTIMIZE, G_REGEX_MATCH_NOTEMPTY, NULL);

    tester->loaded = FALSE;
}

static void
adblock_uri_tester_set_property(GObject* object,
    guint prop_id,
    const GValue* value,
    GParamSpec* pspec)
{
    AdblockUriTester* tester = ADBLOCK_URI_TESTER(object);

    switch (prop_id) {
    case PROP_FILTER_FILE_PATH:
        tester->filter_file_path = g_value_dup_string(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
adblock_uri_tester_finalize(GObject* object)
{
    AdblockUriTester* tester = ADBLOCK_URI_TESTER(object);

    g_free(tester->filter_file_path);

    g_hash_table_destroy(tester->pattern);
    g_hash_table_destroy(tester->keys);
    g_hash_table_destroy(tester->optslist);
    g_hash_table_destroy(tester->urlcache);

    g_hash_table_destroy(tester->whitelisted_pattern);
    g_hash_table_destroy(tester->whitelisted_keys);
    g_hash_table_destroy(tester->whitelisted_optslist);
    g_hash_table_destroy(tester->whitelisted_urlcache);

    g_regex_unref(tester->regex_third_party);
    g_regex_unref(tester->regex_pattern);
    g_regex_unref(tester->regex_subdocument);

    G_OBJECT_CLASS(adblock_uri_tester_parent_class)->finalize(object);
}

static void
adblock_uri_tester_class_init(AdblockUriTesterClass* klass)
{
    GObjectClass* object_class = G_OBJECT_CLASS(klass);

    object_class->set_property = adblock_uri_tester_set_property;
    object_class->finalize = adblock_uri_tester_finalize;

    obj_properties[PROP_FILTER_FILE_PATH] = g_param_spec_string("filter-file-path",
        "Filter file path",
        "Path to the easylist.txt filter file",
        NULL,
        G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

    g_object_class_install_properties(object_class, LAST_PROP, obj_properties);
}

AdblockUriTester*
adblock_uri_tester_new(const char* filter_file_path)
{
    return ADBLOCK_URI_TESTER(g_object_new(ADBLOCK_TYPE_URI_TESTER,
        "filter-file-path", filter_file_path, NULL));
}

void adblock_uri_tester_load(AdblockUriTester* tester)
{
    if (tester->loaded)
        return;
    load_filters_sync(tester);
}

gboolean
adblock_uri_tester_test_uri(AdblockUriTester* tester,
    const char* request_uri,
    const char* page_uri)
{
    if (!tester->loaded)
        return FALSE;

    if (is_matched(tester, request_uri, page_uri, TRUE))
        return FALSE;

    return is_matched(tester, request_uri, page_uri, FALSE);
}
