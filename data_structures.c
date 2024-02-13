#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "data_structures.h"

// You must do free on the returned value
/*const char *node_to_string(const node *n) {
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
}*/

fe4_inode inodes[256];

void init_inodes() {
    inodes[0].stat.st_mode = S_IFDIR | 0777;
    // Size is already 0

    inodes[1].stat.st_mode = S_IFDIR | 0777;

    fe4_dirent de = {.filename = "a", .inode_number = 1};
    inodes[0].stat.st_size = sizeof(fe4_dirent);
    memcpy(inodes[0].contents, &de, sizeof(fe4_dirent));
}

ssize_t read_inode(const fe4_inode *inode, void *buf, size_t count, off_t offset) {
    off_t len = inode->stat.st_size;
	if (offset < len) {
		if (offset + count > len)
			count = len - offset;
        memcpy(buf, inode->contents + offset, count);
	} else
		count = 0;

	return count;
}

int get_inode_from_path_rec(const char *path, fe4_inode *ret, fe4_inode *current_root_inode) {
    if (strcmp(path, "/") == 0) {
        memcpy(ret, current_root_inode, sizeof(fe4_inode));
        return 0;
    }

    return -1;
}

// Return 0 on success, -1 on error
int get_inode_from_path(const char *path, fe4_inode *ret) {
    return get_inode_from_path_rec(path, ret, &inodes[0]);
}

int get_nb_children(const fe4_inode *inode) {
    if (!S_ISDIR(inode->stat.st_mode))
        return -1;

    return inode->stat.st_size / sizeof(fe4_dirent);
}

int get_dirent_at(const fe4_inode *parent, int index, fe4_dirent *ret) {
    read_inode(parent, ret, sizeof(fe4_dirent), index * sizeof(fe4_dirent));

    return 0;
}

int get_inode_at(int index, fe4_inode *ret) {

}