#include <stdio.h>
#include <string.h>

#include "../strings/strings.h"

#define LIBRE_N 50

/* Inspired by https://libredirect.github.io/, but in C. */
int libre_redirect(const char* uri, char* output)
{
    int len_uri = strlen(uri);
    int len_output = strlen(output);

    if ((len_output - len_uri) < LIBRE_N) {
        fprintf(stderr, "Not enough memory\n");
        return 1; // not enough memory.
    } else {
        char* annoying_sites[] = {
            "https://www.reddit.com",
            "https://vitalik.ca",
            "https://www.youtube.com",
            // "https://google.com",
            "https://medium.com",
            "https://translate.google.com",
            "https://www.bloomberg.com",
            "https://www.royalroad.com",
            "https://genius.com",
            // "https://twitter.com"
            // "https://archive.org",
            // "https://twitter.com"
        };

        char* alternatives[] = {
            "https://redlib.tux.pizza",
            // previously: "https://old.reddit.com", "https://teddit.nunosempere.com",
            // https://github.com/redlib-org/redlib-instances/blob/main/instances.md
            "https://vitalik.eth.limo",
            "https://invidious.private.coffee",
            // "https://search.nunosempere.com",
            "https://scribe.rip",
            "https://translate.riverside.rocks",
            "https://archive.ph/https://www.bloomberg.com",
            "https://royalread.nunosempere.com",
            "https://dumb.vern.cc",
            "https://example.com"
            // "https://wayback.nunosempere.com",
            // "https://nitter.net"
        };

        int len = sizeof(annoying_sites) / sizeof(annoying_sites[0]);
        for (int i = 0; i < len; i++) {
            str_init(output, len_output);
            int replace_check = str_replace_start(uri, annoying_sites[i],
                alternatives[i], output);
            switch (replace_check) {
                case 0: // no match found
                    break;
                case 1: // str_replace_start somehow failed
                    fprintf(stderr, "str_replace_start failed\n");
                    return 1;
                    break;
                case 2: // match succeeded
                    return 2;
                    break;
                default:
                    fprintf(stderr, "Unreachable state\n");
            }
        }
        strcpy(output, uri);
    }
    return 0;
}
