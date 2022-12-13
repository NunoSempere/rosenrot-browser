#include "libre_redirect.h"
#include <string.h>
#include <stdio.h>

int main(){
  char uri[] = "https://reddit.com/r/blah";

  int uri_length = strlen(uri);
  int l = LIBRE_N+uri_length;
  char uri_filtered[l++];
  for(int i=0; i<l; i++){
    uri_filtered[i] = ' ';
  }
  uri_filtered[l] = '\0';

  int libre_check = libre_redirect(uri, uri_filtered);
  if(!libre_check){
    printf("Filtered url: %s", uri_filtered);
  }else{
    printf("Uri: %s", uri);
    // failure; do something with the original uri.
  }
}


