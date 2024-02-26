retourner adresse de inode[0] au lieu de memcpy inode[0] dans ret
mettre les dirent . et .. qui existent vraiment dans ext, mettre nlink
trous dans les dirent
dans le code fichiers a b c et toutes les opérations en read et open read
un noeud est un dirent
keep track of nlink

TODO
**static int fe4_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi)** // stat
**static int fe4_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags)**
static int fe4_open(const char *path, struct fuse_file_info *fi)
static int fe4_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)










```c
static int fe4_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi,
			 enum fuse_readdir_flags flags)

fe4_readdir {
    inode = get_inode_from_path(path)
    while readdir {
        ajouter filename du fils à ret
    }
    return ret
}

// Returns 0 on success
// Must work with path == /
int get_inode_from_path(const char *path, fe4_inode *ret, int current_root_inode);

get_inode_from_path(const char *path, fe4_inode *ret, int current_root_inode) {
    if path == / return inodes[current_root_inode]

    while fils = readdir(inodes[current_root_inode]) {
        if fils.filename == 1er elem du path (pour /abc, abc) {
            if 1er elem du path est aussi le dernier {
                return fils.inode
            } else {
                return get_inode_from_path(path + x, ret, fils.inode)
            }
        }
    }

    return -1
}
``` 