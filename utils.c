#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

char *basename(char *path) {
    char *last_slash = strrchr(path, '/');
    if (last_slash == NULL) {
        return path;
    } else {
        return last_slash + 1;
    }
}

/*char *basename(char *absolute_path){
  char *res  = malloc(255);
  memset(res,0,255);
  if (strlen(absolute_path) == 1 && absolute_path[0] == '/'){
    res[0] = '/';
  } else if (absolute_path[0] != '/') {
    res = NULL;
  } else if (strlen(absolute_path) > 1){
    int start_index = strlen(absolute_path) - 1;
    if(absolute_path[start_index-1] == '/') {
      start_index--;
    }
    int i = 0;
    while(absolute_path[start_index-1] != '/'){
      start_index--;
      i++;
    }
    int j = 0;
    while(j < i){
      res[j] = absolute_path[start_index];
      j++;
      start_index++;
    }
  }
  return res;
}*/
