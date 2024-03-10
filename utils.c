#include <stdlib.h>
#include <string.h>
#include <libgen.h>

/*
 * Returned string is null-terminated and needs to be freed by the caller.
 */
/*char *dirname2(const char *path) {
    char *pathcpy = malloc(strlen(path) + 1);
    strcpy(pathcpy,path);
    char *dir = dirname(pathcpy);
    char *res = malloc(strlen(dir) + 1);
    strcpy(res,dir);
    free(pathcpy);
    return res;
}*/

/*
 * Returned string is null-terminated and needs to be freed by the caller.
 */
/*char *basename2(const char *path) {
    char *pathcpy = malloc(strlen(path) +1);
    strcpy(pathcpy,path);
    char *dir = basename(pathcpy);
    char *res = malloc(strlen(dir) +1);
    strcpy(res,dir);
    free(pathcpy);
    return res;
}*/

/*
 * Returns a non const copy of path processed by func and which is not freed
 */
char *process_path(const char *path, char *(*func)(char *)) {
    char *pathcpy = malloc(strlen(path) + 1);
    strcpy(pathcpy,path);
    char *dir = func(pathcpy);
    char *res = malloc(strlen(dir) + 1);
    strcpy(res,dir);
    free(pathcpy);
    return res;
}

/*
 * Returned string is null-terminated and needs to be freed by the caller.
 */
char *dirname2(const char *path) {
    return process_path(path,  dirname);
}

/*
 * Returned string is null-terminated and needs to be freed by the caller.
 */
char *basename2(const char *path) {
    return process_path(path,  basename);
}
