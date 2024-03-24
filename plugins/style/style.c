#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STYLE_N 7827 + 1000

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
        string[i++] = c;
    }
    string[i] = '\0';
    fclose(fp);
}
