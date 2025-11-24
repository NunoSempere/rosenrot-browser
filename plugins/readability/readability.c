#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define READABILITY_N 88212 + 1000

void read_readability_js(char* string)
{
    gchar* file_contents = NULL;
    gsize length = 0;
    GError* error = NULL;

    if (!g_file_get_contents("/opt/rosenrot/readability.js", &file_contents, &length, &error)) {
        fprintf(stderr, "Failed to open file: %s\n", error ? error->message : "unknown error");
        fprintf(stderr, "Consider running $ sudo make runtime_files\n");
        if (error) g_error_free(error);
        string[0] = '\0';
        return;
    }

    if (length > READABILITY_N) {
        fprintf(stderr, "readability.js file is too large (%zu bytes, max %d)\n", length, READABILITY_N);
        fprintf(stderr, "Consider increasing READABILITY_N or running recompute_READABILITY_N.sh\n");
        g_free(file_contents);
        string[0] = '\0';
        return;
    }

    memcpy(string, file_contents, length);
    string[length] = '\0';
    g_free(file_contents);
}

/*
int main(){
    char* readability_js = malloc(READABILITY_N+1);
    read_readability_js(readability_js);
    printf("%s", readability_js);
    free(readability_js);
}
*/
