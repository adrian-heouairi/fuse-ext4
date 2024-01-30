#include <stdlib.h>
#include <string.h>

#include "data_structures.h"

#define DEFAULT_NB_CHILDREN 256

// node *home_made_mkdir(const char *path, mode_t mode);
// node *get_entry(const char *path);
// int add_entry(const node* node);
// int rm_entry(node* node);

// map_entry *entries;

node *create_node(const char *path, int file_type, int permissions) {
    node *ret = malloc(sizeof(node) + DEFAULT_NB_CHILDREN - 1);
    memset(ret, 0, sizeof(node) + DEFAULT_NB_CHILDREN - 1);
    strcpy(ret->path, path);
    ret->info.st_mode = file_type | permissions;
    ret->nb_children = 0;
    ret->max_nb_children = 256;

    return ret;
}

void add_child(node *parent, node *child) {
    int i = 0;
    while (parent->children + i != 0) i++;

    node *new_child_pos = parent->children + i;
    new_child_pos = child;

    parent->nb_children++;
}

node *get_node_from_path(const char *path, node *current) {
    if (strcmp(path, current->path) == 0)
        return current;

    for (int i = 0; i < current->nb_children; i++) {
        node *ret = get_node_from_path(path, current->children + i);

        if (ret != 0)
            return ret;
    }

    return 0;
}
