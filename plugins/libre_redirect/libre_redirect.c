#include "str_replace_start.h"
#include <string.h>
#define LIBRE_N 12

/* Inspired by https://libredirect.github.io/, but in C. */

// int str_replace_start(const char* string, const char* target, const char* replacement, char* output){

void str_init(char* str, int n){
  for(int i=0; i<n; i++){
    str[i] = ' ';
  }
  str[n] = '\0';
} // could also use <https://manpages.ubuntu.com/manpages/impish/man3/strinit.3pub.html>

int libre_redirect(const char* uri, char* output){
  /* inv.riverside.rocks */
  // max length
  int l1 = strlen(uri);
  int l2 = strlen(output);
  
  if((l2 - l1) < LIBRE_N){ 
    return 1; // not enough memory. 
  }else{
    char tmp_uri[l2++];
    strcpy(tmp_uri, uri); // includes terminating '\0'

    char* sites[] = { 
      "https://youtube.com", 
      "https://reddit.com", 
      "https://medium.com", 
      "https://translate.google.com" 
    };
    char* alternatives[] = { 
      "https://yt.artemislena.eu", 
      "https://teddit.nunosempere.com", 
      "https://scribe.rip", 
      "https://simplytranslate.org/" 
    };

    for(int i=1; i<4; i++){
      int replace_check = str_replace_start(tmp_uri, sites[i], alternatives[i], output);
      if(replace_check){
        return 1;
      }
      strcpy(tmp_uri, output);
    }
    strcpy(output, tmp_uri);
  }

  return 0;
  
}
