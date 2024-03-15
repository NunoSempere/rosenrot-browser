#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../libre_redirect/str_init.h"
#include "../libre_redirect/str_replace_start.h"

#define SHORTCUT_N 41

/* Uncomment for debug */
/* #define DEBUG */

/* Inspired by https://duckduckgo.com/bangs */

int shortcut_expand(const char* uri, char* output)
{
    printf("SHORTCUT EXPAND!\n");
    int l1 = strlen(uri);
    int l2 = strlen(output);
    int len;
    char tmp_uri[l2++];
    char tmp_output[l2++];

    if ((l2 - l1) < SHORTCUT_N) {
#ifdef DEBUG
        printf("Not enough memory\n");
#endif
        return 1; // not enough memory.
    } else {
        strcpy(tmp_uri, uri); // strcpy also copies the terminating '\0'
        strcpy(tmp_output, output);

        char* shortcuts[] = {
            "!x",
            "!fnf",
            "!fnc",
            "!hn",
            "!hnb"
        };

        char* expansions[] = {
            "https://twitter.com"
            "https://forum.nunosempere.com/frontpage",
            "https://forum.nunosempere.com/comments",
            "https://news.ycombinator.com",
            "https://news.ycombinator.com/best",
        };

        // len = sizeof(shortcuts) / sizeof(shortcuts[0]);
        len = sizeof(shortcuts) / sizeof(char*);

        for (int i = 0; i < len; i++) {
            int replace_check = str_replace_start(tmp_uri, shortcuts[i],
                expansions[i], output);
            if (replace_check == 2) {
#ifdef DEBUG
                printf("tmp_uri: %s\n", tmp_uri);
                printf("output: %s\n", output);
#endif
                return 2;
            } else if (replace_check == 1) {
#ifdef DEBUG
                printf("replace_check failed\n");
#endif
                return 1;
            }
            strcpy(tmp_uri, output);
            str_init(output, l2);
        }
        strcpy(output, tmp_uri);
    }
#ifdef DEBUG
    printf("No match found\n\n");
#endif
    return 0;
}
