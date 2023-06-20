#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STYLE_N 3986 + 1

void read_style_js(char* string)
{
    FILE* fp = fopen("/home/loki/Documents/core/software/fresh/C/rose-browser/rosenrot/plugins/style/style.js", "r");
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

/*
int main(){
    char* readability_js = malloc(STYLE_N+1);
    read_readability_js(readability_js);
    printf("%s", readability_js);
    free(readability_js);
}
*/
