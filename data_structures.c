#include "data_structures.h"

node *home_made_mkdir(const char *path, mode_t mode);
node *get_entry(const char *path);
int add_entry(const node* node);
int rm_entry(node* node);
