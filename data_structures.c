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

// For now, should set: st_ino st_uid st_gid --- st_mode st_nlink st_size
// Dirents should have . and .. as first two entries (manually added)
void init_inodes() {
    //mode_t umask_arg = umask(022);
    //umask(umask_arg);
    //inodes[ROOT_INODE].stat.st_mode = S_IFDIR | ~umask_arg;

    inodes[ROOT_INODE].stat.st_mode = S_IFDIR;
    inodes[ROOT_INODE].stat.st_nlink = 2;
    inodes[ROOT_INODE].stat.st_size = 3  * sizeof(fe4_dirent);
    fe4_dirent parent = {.filename = "..", .inode_number = ROOT_INODE};
    fe4_dirent current = {.filename = ".", .inode_number = ROOT_INODE};
    fe4_dirent de = {.filename = "a", .inode_number = 1};
    memcpy(inodes[ROOT_INODE].contents, &parent, sizeof(fe4_dirent));
    memcpy(inodes[ROOT_INODE].contents + sizeof(fe4_dirent), &current, sizeof(fe4_dirent));
    memcpy(inodes[ROOT_INODE].contents + 2 * sizeof(fe4_dirent), &de, sizeof(fe4_dirent));
    
    inodes[1].stat.st_mode = S_IFREG;
    inodes[1].stat.st_nlink = 2;
    inodes[1].stat.st_size = 2;
    inodes[1].contents[0] = 'a';
    inodes[1].contents[1] = 'b';

    for (int i = 0; i < 256; i++) {
        inodes[i].stat.st_ino = i;
        inodes[i].stat.st_uid = getuid();
        inodes[i].stat.st_gid = getgid();
        //inodes[i].stat.st_size = -1; // -1 for unused

        inodes[i].stat.st_mode |= 0755;
    }
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

int get_inode_from_path(const char *path, fe4_inode *ret) {
    // We assume that path is a valid string
    if (strlen(path) == 0 || path[0] != '/')
        return -1;
    
    if (strcmp(path, "/") == 0) {
        //memcpy(ret, &inodes[0], sizeof(fe4_inode));
        get_inode_at(0, ret);
        return 0;
    }

    // TODO Handle path == "//"

    char *path_copy = malloc(strlen(path) + 1);
    strcpy(path_copy, path);

    fe4_inode *parent = &inodes[0];
    char *child = strtok(path_copy, "/");
    char *grandchild = strtok(NULL, "/");

    l:
    for (int i = 0; i < get_nb_children(parent); i++) {
        fe4_dirent dirent;
        get_dirent_at(parent, i, &dirent);

        if (strcmp(dirent.filename, child) == 0) {
            if (grandchild == NULL) {
                get_inode_at(dirent.inode_number, ret);
                
                free(path_copy);
                return 0;
            }

            get_inode_at(dirent.inode_number, parent);
            child = grandchild;
            grandchild = strtok(NULL, "/");
            goto l;
        }
    }

    free(path_copy);
    return -1;
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
    memcpy(ret, &inodes[index], sizeof(fe4_inode));

    return 0;
}
