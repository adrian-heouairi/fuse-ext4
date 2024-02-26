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

// int get_inode_from_path_rec(const char *path, char *tmp_path,fe4_inode *ret, fe4_inode *current_root_inode) {
//     // tmp_path = "/"
//     // path que je cherche strtok coupe par rapport a "/"
//     // path que je cherche  premier token cheeche dans les fils de tmp_path puis virer ce token

//     if (strcmp(path, "/") == 0) {
//         memcpy(ret, current_root_inode, sizeof(fe4_inode));
//         return 0;
//     }

//     return -1;
// }

// Return 0 on success, -1 on error
// int get_inode_from_paths(const char *path, fe4_inode *ret) {
//     // if path == "/" return inodes[0]
//     char *tokens = strtok(path, "/");
//     char *tmp_path = malloc(256 * sizeof(char));
//     int j = 0;
//     while (tokens = strtok(NULL, "/") != NULL)
//     {
//         strcat(tmp_path, "/");
//         fe4_dirent currnt_dirent;
//         for (size_t i = 0; i < get_nb_children(&inodes[j]); i++)
//         {
//             int r = get_dirent_at(&inodes[j], i, &currnt_dirent);
//             if (currnt_dirent.filename == tokens)
//             {
//                 ret = &currnt_dirent;
//                 strcat(tmp_path, tokens);
//             }
//             else
//             {
//                 continue;
//             }
//         }
//     }
//     free(tmp_path);
//     return 0;
// }

    // if path ne start pas par / return -1
    // if path == / return inodes[0]

    // parent = inode de / = inodes[0]
    // child = strtok(path, "/") e.g. a dans /a/b/c
    // next = strtok(path, "/") e.g. b dans /a/b/c

    // while (1) {
    //     for i in 0..get_nb_children(parent) {
    //         if child == get_dirent_at(parent, i).filename {
    //             if next == null: return child
    //             parent = inode de child
    //             child = next
    //             next = strtok(NULL, "/")
    //             break
    //         }
    //     }
    //     return -1
    // }

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
