#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STYLE_N 9404 + 1000

void read_style_js(char* string)
{
    gchar* file_contents = NULL;
    gsize length = 0;
    GError* error = NULL;

    if (!g_file_get_contents("/opt/rosenrot/style.js", &file_contents, &length, &error)) {
        fprintf(stderr, "Failed to open file: %s\n", error ? error->message : "unknown error");
        if (error) g_error_free(error);
        string[0] = '\0';
        return;
    }

    if (length > STYLE_N) {
        fprintf(stderr, "style.js file is too large (%zu bytes, max %d)\n", length, STYLE_N);
        fprintf(stderr, "Consider increasing STYLE_N or running recompute_STYLE_N.sh\n");
        g_free(file_contents);
        string[0] = '\0';
        return;
    }

    memcpy(string, file_contents, length);
    string[length] = '\0';
    g_free(file_contents);
}
