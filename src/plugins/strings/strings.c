#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define DEBUG false

// String manipulation
void str_init(char* str, int n)
{
    // could also use <https://manpages.ubuntu.com/manpages/impish/man3/strinit.3pub.html>
    for (int i = 0; i < n; i++)
        str[i] = ' ';
    str[n-1] = '\0';
}
int str_replace_start(const char* string, const char* target, const char* replacement, char* output)
{
    int l1 = strlen(string);
    int l2 = strlen(target);
    int l3 = strlen(replacement);
    int l4 = strlen(output);

    if (DEBUG) {
        printf("string: %s, target: %s, replacement: %s, output: %s\n", string, target, replacement, output);
        printf("%d,%d,%d,%d\n", l1, l2, l3, l4);
    }

    if ((l4 < (l1 - l2 + l3)) || l4 < l1) {
        printf("Not enough memory in output string.\n");
        return 1;
    }
    int match = true;
    for (int i = 0; i < l2; i++) {
        if (string[i] != target[i]) {
            match = false;
            break;
        }
    }
    if (match) {
        if (DEBUG) printf("Found match.\n");
        for (int i = 0; i < l3; i++) {
            output[i] = replacement[i];
        }
        int counter = l3;
        for (int i = l2; i < l1; i++) {
            output[counter] = string[i];
            counter++;
        }
        output[counter] = '\0';
        return 2; // success
    } else {
        if (DEBUG) printf("Did not find match.\n");
        // Use strncpy with explicit null termination for safety
        strncpy(output, string, l4 - 1);
        output[l4 - 1] = '\0';
    }

    return 0;
}
/*
See also:
* <https://web.archive.org/web/20160201212501/coding.debuntu.org/c-implementing-str_replace-replace-all-occurrences-substring>
* https://github.com/irl/la-cucina/blob/master/str_replace.c
*/


int str_destructively_omit_from(char* input, const char* from){
    // input = "https://url.com/?utm=blah", from = "?utm"
    int l1 = strlen(input);
    int l2 = strlen(from);

    for(int i=0; i<l1; i++){
        if((i + l2) > l1) { // no more room 
            continue;
        }
        for(int j=0; j<l2; j++){
            if(input[i+j] != from[j]){
                goto cont;
            }
        }
        input[i]='\0';
        printf("Replaced utm %s\n", input);
        return 1;
        cont:;
    }
    return 0;
}
