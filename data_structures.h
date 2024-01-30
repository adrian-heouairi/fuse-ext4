#ifndef DATA_STRUCTURES
#include <sys/stat.h>
typedef struct node {
  char path [256];
  struct stat *info;
} node;
typedef struct map_entry {
  char path [256];
  node *element;
} map_entry;
/* *
 * Constains all entries of our file system be they files or directories.
 * */
map_entry *entries;

/* *
 * Number of entries in our file system.
 * */
#define ENTRIES_MAP_SIZE 50

/* *
 * Creates a directory allocates it and adds it to the entries. 
 * */
node *home_made_mkdir(const char *path, mode_t mode);

/* *
 * Returns the node associatied to the given path NULL if none. 
 * */
node *get_entry(const char *path);

/* *
 * Adds the node to the map allocates memory based on dynamic arrays. 
 * It ensures set behaviour.
 * Returns -1 if entry exists and does not touch the data structure.
 * Sets errno consequently.
 * */
int add_entry(const node* node);

/* *
 * Removes the node to the map and frees memory.
 * Sets element place to NULL.
 * Shrinks entries map size if less than fourth of the places are full.
 * Returns -1 if entry does not exists or it is impossible to remove it.
 * Sets errno consequently.
 * */
int rm_entry(node* node);
#endif // !DATA_STRUCTURES
