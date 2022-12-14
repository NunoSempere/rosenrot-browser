#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define TRANQUILITY_N 46288

void read_tranquility_js(char* string){
  FILE *fp=fopen("/home/loki/Documents/core/software/fresh/C/rose-browser/rose-bud-personal/plugins/tranquility/snippet.js", "r");
if (!fp) { // fp is NULL, fopen failed
  fprintf(stderr, "Failed to open file\n");
  string=NULL;
  return;
}
  int i=0;
  int c;
  while ((c = fgetc(fp)) != EOF){
     string[i++] = c;
  }
  string[i]='\0';
  fclose(fp);
}

/*
int main(){
    char* tranquility_js = malloc(TRANQUILITY_N+1);
    read_tranquility_js(tranquility_js);
    printf("%s", tranquility_js);
    free(tranquility_js);

}
*/
