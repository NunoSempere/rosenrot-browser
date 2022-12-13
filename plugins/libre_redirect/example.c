#include "libre_redirect.h"
#include <string.h>
#include <stdio.h>

int main(){
  char uri[] = "https://reddit.com/r/blah";
  
  int l = LIBRE_N + strlen(uri) + 1;
  char uri_filtered[l];
  str_init(uri_filtered, l);

  int libre_check = libre_redirect(uri, uri_filtered);
  if(!libre_check){
    printf("Filtered url: %s\n", uri_filtered);
  }else{
    printf("Uri: %s\n", uri);
    // failure; do something with the original uri.
  }
}


