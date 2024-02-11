#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "data_structures.h"

#define DEFAULT_MAX_NB_CHILDREN 256

node *create_node(const char *path, int file_type, int permissions) {
    int struct_size = sizeof(node) + DEFAULT_MAX_NB_CHILDREN * sizeof(node *);

    node *ret = malloc(struct_size);
    memset(ret, 0, struct_size);
    strcpy(ret->path, path);
    ret->info.st_mode = file_type | permissions;
    ret->nb_children = 0;
    ret->max_nb_children = DEFAULT_MAX_NB_CHILDREN;

    return ret;
}

void add_child(node *parent, node *child) {
    int i = 0;
    while (parent->children[i] != NULL) i++;

    parent->children[i] = child;

    parent->nb_children++;
}

node *get_node_from_path(const char *path, node *current) {
    if (strcmp(path, current->path) == 0)
        return current;

    for (int i = 0; i < current->nb_children; i++) {
        node *ret = get_node_from_path(path, current->children[i]);

        if (ret != NULL)
            return ret;
    }

    return NULL;
}

// You must do free on the returned value
const char *node_to_string(const node *n) {
    char *ret = malloc(8192 * sizeof(char));

    if (n == NULL) {
        sprintf(ret, "Node is NULL");
        return ret;
    }

    sprintf(ret, "path: %s ", n->path);
    sprintf(ret + strlen(ret), "nb_children: %d ", n->nb_children);
    sprintf(ret + strlen(ret), "max_nb_children: %d ", n->max_nb_children);
    for (int i = 0; i < 3; i++) {
        sprintf(ret + strlen(ret), "child %d: %p ", i, (void *)n->children[i]);
    }

    return ret;
}
