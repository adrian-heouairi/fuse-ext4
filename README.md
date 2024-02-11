# fuse-ext4

Dependencies:

`sudo apt install libfuse3-dev`

Creation of test.ext4:

```
fallocate -l 15M test.ext4
mkfs.ext4 test.ext4
mkdir test
sudo mount test.ext4 test
```

We parse test.ext4 which contains an ext4 file system with the following files:

```
/a (directory)
/a/b (directory)
/a/b/{jkl} (empty file)
/lost+found
/with spaces (empty file)
/x (regular file with contents "hello world\n")
/y (empty file)
/z (empty file)
```

# TODO

FUSE functions to implement:

```
int fe4_mknod(const char *path, mode_t mode, dev_t dev)
static void *fe4_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
static int fe4_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) // stat
static int fe4_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags)
static int fe4_open(const char *path, struct fuse_file_info *fi)
static int fe4_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
```

```
int (*mkdir) (const char *, mode_t);
int (*unlink) (const char *);
int (*rmdir) (const char *);
int (*rename) (const char *, const char *, unsigned int);
int (*chmod) (const char *, mode_t);
int (*chown) (const char *, uid_t, gid_t);
int (*write) (const char *, const char *, size_t, off_t, struct fuse_file_info *);
int (*truncate) (const char *, off_t);
```
