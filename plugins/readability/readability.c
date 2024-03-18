#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define READABILITY_N 88067 + 1000

void read_readability_js(char* string)
{
    FILE* fp = fopen("/opt/rosenrot/readability.js", "r");
    if (!fp) { // fp is NULL, fopen failed
        fprintf(stderr, "Failed to open file\n");
        fprintf(stderr, "Consider running $ sudo make runtime_files\n");
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
    char* readability_js = malloc(READABILITY_N+1);
    read_readability_js(readability_js);
    printf("%s", readability_js);
    free(readability_js);
}
*/
