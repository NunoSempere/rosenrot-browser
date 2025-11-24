#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STYLE_N 9404 + 1000

void read_style_js(char* string)
{
    FILE* fp = fopen("/opt/rosenrot/style.js", "r");
    if (!fp) { // fp is NULL, fopen failed
        fprintf(stderr, "Failed to open file\n");
        string = NULL;
        return;
    }
    int i = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (i >= STYLE_N) {
            fprintf(stderr, "style.js file is too large (exceeds %d bytes)\n", STYLE_N);
            fprintf(stderr, "Consider increasing STYLE_N or running recompute_STYLE_N.sh\n");
            fclose(fp);
            string[0] = '\0';
            return;
        }
        string[i++] = c;
    }
    string[i] = '\0';
    fclose(fp);
}
