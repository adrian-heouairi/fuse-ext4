#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "data_structures.h"
//#include "sprintstatf.h"

void print_inode(const fe4_inode *inode) {
    // sprintf(ret + strlen(ret), "nb_children: %d ", n->nb_children);

    printf("inode: %p\n", (void *)inode);
    printf("inode->stat.st_ino: %lu\n", inode->stat.st_ino);
    printf("inode->stat.st_uid: %u\n", inode->stat.st_uid);
    printf("inode->stat.st_gid: %u\n", inode->stat.st_gid);
    printf("inode->stat.st_mode: %u\n", inode->stat.st_mode);
    printf("inode->stat.st_nlink: %lu\n", inode->stat.st_nlink);
    printf("inode->stat.st_size: %lu\n", inode->stat.st_size);
}

fe4_inode inodes[256];

ino_t get_next_free_inode_number(void) {
    for (int i = 0; i < 256; i++) {
        if (inodes[i].stat.st_size == -1) {
            return i;
        }
    }

    return ENOSPC; // TODO Error handling
}

// For now, should set: st_ino st_uid st_gid --- st_mode st_nlink st_size
// Dirents should have . and .. as first two entries (manually added)
void init_inodes(void) {
    for (int i = 0; i < 256; i++)
        inodes[i].stat.st_size = -1; // -1 for unused

    //mode_t umask_arg = umask(022);
    //umask(umask_arg);
    //inodes[ROOT_INODE].stat.st_mode = S_IFDIR | ~umask_arg;

    //inodes[ROOT_INODE].stat.st_mode |= S_IFDIR;
    //inodes[ROOT_INODE].stat.st_nlink = 2;
    get_new_dir_inode(ROOT_INODE);
}

fe4_inode *get_new_dir_inode(ino_t parent_inode_number) {
    ino_t new_inode_number = get_next_free_inode_number();

    fe4_inode *inode = &inodes[new_inode_number];

    memset(inode, 0, sizeof(fe4_inode)); // Size is set to 0
    inode->stat.st_ino = new_inode_number;
    inode->stat.st_uid = getuid();
    inode->stat.st_gid = getgid();
    inode->stat.st_mode = S_IFDIR | 0755;

    inode->stat.st_nlink = 2;

    fe4_dirent current = {.filename = ".", .inode_number = new_inode_number};
    fe4_dirent parent = {.filename = "..", .inode_number = parent_inode_number};
    append_dirent_to_inode(inode, &current);
    append_dirent_to_inode(inode, &parent);

    return inode;
}

fe4_inode *get_new_file_inode(void) {
    ino_t new_inode_number = get_next_free_inode_number();

    fe4_inode *inode = &inodes[new_inode_number];

    memset(inode, 0, sizeof(fe4_inode)); // Size is set to 0
    inode->stat.st_ino = new_inode_number;
    inode->stat.st_uid = getuid();
    inode->stat.st_gid = getgid();
    inode->stat.st_mode = S_IFREG | 0755;

    inode->stat.st_nlink = 1;

    return inode;
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

ssize_t write_inode(fe4_inode *inode, const void *buf, size_t count, off_t offset) {
    off_t len = inode->stat.st_size;
    if (offset + count > len) {
        inode->stat.st_size = offset + count;
    }
    memcpy(inode->contents + offset, buf, count);

    return count;
}

fe4_inode *get_inode_from_path(const char *path) {
    // We assume that path is the path asked by FUSE (it is absolute, there are no trailing slashes, it doesn't contain "//", etc.)
    
    if (strcmp(path, "/") == 0) {
        return get_inode_at(ROOT_INODE);
    }

    char *path_copy = malloc(strlen(path) + 1);
    strcpy(path_copy, path);

    fe4_inode *parent = &inodes[ROOT_INODE];
    char *child = strtok(path_copy, "/");
    char *grandchild = strtok(NULL, "/");

    l:
    for (int i = 0; i < get_nb_children(parent); i++) {
        const fe4_dirent *dirent = get_dirent_at(parent, i);

        if (strcmp(dirent->filename, child) == 0) {
            if (grandchild == NULL) {
                free(path_copy);
                return get_inode_at(dirent->inode_number);
            }

            parent = get_inode_at(dirent->inode_number);
            child = grandchild;
            grandchild = strtok(NULL, "/");
            goto l;
        }
    }

    free(path_copy);
    return NULL;
}

int get_nb_children(const fe4_inode *inode) {
    if (!S_ISDIR(inode->stat.st_mode))
        return -1;

    return inode->stat.st_size / sizeof(fe4_dirent);
}

fe4_dirent *get_dirent_at(fe4_inode *parent, int index) {
    return (fe4_dirent *)(parent->contents + index * sizeof(fe4_dirent));
}

fe4_inode *get_inode_at(ino_t index) {
    return &inodes[index];
}

void append_dirent_to_inode(fe4_inode *inode, const fe4_dirent *dirent) {
    int nb_children = get_nb_children(inode);
    memcpy(inode->contents + nb_children * sizeof(fe4_dirent), dirent, sizeof(fe4_dirent));
    inode->stat.st_size += sizeof(fe4_dirent);
}
