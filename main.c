#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <libgen.h>

#include "data_structures.h"
#include "utils.h"

/*
 * Command line options
 *
 * We can't set default values for the char* fields here because
 * fuse_opt_parse would attempt to free() them when the user specifies
 * different values on the command line.
 */
static struct options {
	int show_help;
} options;

/*int fe4_mknod(const char *path, mode_t mode, dev_t dev) {
	fuse_log(FUSE_LOG_INFO, "mknod started\n");
    char *path_for_dirname = malloc(strlen(path) +1);
    strcpy(path_for_dirname,path);
	fe4_inode in;
	int r = get_inode_from_path(path, &in);
    fuse_log(FUSE_LOG_INFO, "after retriving inode\n");
	if (r == -1) {
		fe4_inode *next_inode = get_next_free_inode();
		if (next_inode == NULL) {
			fuse_log(FUSE_LOG_ERR, "No more inodes available\n");
			return -ENOSPC;
		}
		next_inode->stat.st_mode = mode;
		next_inode->stat.st_dev = dev;
		next_inode->stat.st_nlink = 1;
		next_inode->stat.st_uid = getuid();
		next_inode->stat.st_gid = getgid();
        fuse_log(FUSE_LOG_INFO, "after filling inode\n");
		fe4_inode parent;
		int res = get_inode_from_path(dirname(path_for_dirname),&parent);
        fuse_log(FUSE_LOG_INFO, "GETTING PARENT INODE...");
		if (res == -1) {
            fuse_log(FUSE_LOG_INFO, "GETTING PARENT INODE FAILED");
			fuse_log(FUSE_LOG_ERR, "No parent found\n");
			return -ENOENT;
		} else if (!S_ISDIR(parent.stat.st_mode)) {
			fuse_log(FUSE_LOG_ERR, "Parent is not a directory\n");
			return -ENOTDIR;
		} else {
			fe4_dirent de = {.inode_number = next_inode->stat.st_ino};
            
            memcpy(de.filename,basename(path_for_dirname), strlen(basename(path_for_dirname)));
			memcpy(parent.contents + parent.stat.st_size, &de, sizeof(fe4_dirent));
			parent.stat.st_size += sizeof(fe4_dirent);
            fuse_log(FUSE_LOG_INFO, "after adding dirent to parent\n");
		}

	}
	// TODO: hanle errors e.g. user can not create file because of permissions or no space left on dev.
	// TODO: make sure that time is updated when we implement it
	fuse_log(FUSE_LOG_INFO, "mknod ended\n");

	return 0;
}*/

int fe4_mkdir(const char *path, mode_t mode) {
    fe4_inode *already_there = get_inode_from_path(path);

    if (already_there != NULL)
        return -EEXIST;

    char *path_for_dirname = malloc(strlen(path) + 1);
    strcpy(path_for_dirname, path);
    fe4_inode *parent = get_inode_from_path(dirname(path_for_dirname));
    free(path_for_dirname);

    if (parent == NULL)
        return -ENOENT;

    if (!S_ISDIR(parent->stat.st_mode))
        return -ENOTDIR;

    fe4_inode *child = get_new_dir_inode(parent->stat.st_ino); // TODO Add . and ..

    char *path_for_basename = malloc(strlen(path) + 1);
    strcpy(path_for_basename, path);
    fe4_dirent de = {.inode_number = child->stat.st_ino};
    strcpy(de.filename, basename(path_for_basename));
    free(path_for_basename);

    append_dirent_to_inode(parent, &de);

    return 0;
}

int fe4_mknod(const char *path, mode_t mode, dev_t dev) {
	if (!S_ISREG(mode)) {
		fuse_log(FUSE_LOG_INFO, "Unsupported type requested in mknod\n");
		return -EINVAL;
	}

	fe4_inode *already_there = get_inode_from_path(path);

	if (already_there != NULL)
		return -EEXIST;
		
	char *path_for_dirname = malloc(strlen(path) + 1);
    strcpy(path_for_dirname, path);
	fe4_inode *parent = get_inode_from_path(dirname(path_for_dirname));
	free(path_for_dirname);

	if (parent == NULL)
		return -ENOENT;

	if (!S_ISDIR(parent->stat.st_mode))
		return -ENOTDIR;

	fe4_inode *child = get_new_file_inode();

    char *path_for_basename = malloc(strlen(path) + 1);
    strcpy(path_for_basename, path);
    fe4_dirent de = {.inode_number = child->stat.st_ino};
    strcpy(de.filename, basename(path_for_basename));
    free(path_for_basename);

    append_dirent_to_inode(parent, &de);

    return 0;
}

#define OPTION(t, p) { t, offsetof(struct options, p), 1 }

static const struct fuse_opt option_spec[] = {
	OPTION("-h", show_help),
	OPTION("--help", show_help),
	FUSE_OPT_END
};

static void *fe4_init(struct fuse_conn_info *conn,
			struct fuse_config *cfg) {
	(void) conn;
	cfg->kernel_cache = 1;
	return NULL;
}

static int fe4_getattr(const char *path, struct stat *stbuf,
			 struct fuse_file_info *fi) {
	fuse_log(FUSE_LOG_INFO, "getattr started with path = %s\n", path);

	(void) fi;

	fe4_inode *in = get_inode_from_path(path);

	//fuse_log(FUSE_LOG_INFO, "getattr requested %s: %\n", path, node_to_string(ret));

	if (in == NULL)
		return -ENOENT;

	memcpy(stbuf, &in->stat, sizeof(struct stat));

	return 0;
}

static int fe4_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi,
			 enum fuse_readdir_flags flags) {
	(void) offset;
	(void) fi;
	(void) flags;

	fe4_inode *in = get_inode_from_path(path);

	if (in == NULL)
		return -ENOENT;

	if (!S_ISDIR(in->stat.st_mode))
		return -ENOENT;
		
	for (int i = 0; i < get_nb_children(in); i++) {
		fe4_dirent *de = get_dirent_at(in, i);

		filler(buf, de->filename, NULL, 0, 0);
	}

	//fuse_log(FUSE_LOG_INFO, "readdir requested %s: %s\n", path, node_to_string(ret));

	return 0;
}

static int fe4_open(const char *path, struct fuse_file_info *fi) {
	fe4_inode *in = get_inode_from_path(path);

	if (in == NULL)
		return -ENOENT;

	if (!S_ISREG(in->stat.st_mode))
		return -ENOENT;

//	if ((fi->flags & O_ACCMODE) != O_RDONLY)
//		return -EACCES;

	return 0;
}

static int fe4_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi) {
	fe4_inode *in = get_inode_from_path(path);

	if (in == NULL)
		return -ENOENT;

	if (!S_ISREG(in->stat.st_mode))
		return -ENOENT;

	return read_inode(in, buf, size, offset);
}

static int fe4_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    fe4_inode *in = get_inode_from_path(path);

    if (in == NULL)
        return -ENOENT;

    if (!S_ISREG(in->stat.st_mode))
        return -ENOENT;

    return write_inode(in, buf, size, offset);
}

/*int fe4_chmod(const char *path, mode_t mode, struct fuse_file_info *fi) {
	fe4_inode in;
	fe4_inode *r = get_inode_from_path(path);

	if (r == NULL)
		return -ENOENT;

	in.stat.st_mode = mode;

	return 0;
}

int fe4_chown(const char *path, uid_t uid, gid_t gid, struct fuse_file_info *fi) {
	fe4_inode in;
	fe4_inode *r = get_inode_from_path(path);

	if (r == NULL)
		return -ENOENT;

	in.stat.st_uid = uid;
	in.stat.st_gid = gid;

	return 0;
}

int	fe4_truncate(const char *path, off_t size, struct fuse_file_info *fi) {
	fe4_inode in;
	int r = get_inode_from_path(path, &in);

	if (r == -1)
		return -ENOENT;

	if (!S_ISREG(in.stat.st_mode))
		return -ENOENT;

	if (size > in.stat.st_size) {
		memset(in.contents + in.stat.st_size, 0, size - in.stat.st_size);
	} else {
		in.stat.st_size = size;
	}

	write_inode_at(in.stat.st_ino, &in);

	return 0;
}*/
/*
int fe4_write(const char *path, const char *buf, size_t size, off_t offset,
		       struct fuse_file_info *fi) {
	fe4_inode in;
	int r = get_inode_from_path(path, &in);

	if (r == -1)
		return -ENOENT;

	if (!S_ISREG(in.stat.st_mode))
		return -ENOENT;

	if (offset + size > in.stat.st_size) {
		in.stat.st_size = offset + size;
	}

	memcpy(in.contents + offset, buf, size);

	write_inode_at(in.stat.st_ino, &in);

	return size;
}*/

static const struct fuse_operations fe4_oper = {
	.init           = fe4_init,
	.getattr	= fe4_getattr,
	.readdir	= fe4_readdir,
	.open		  = fe4_open,
	.read		  = fe4_read,
    .mknod    = fe4_mknod,
    .mkdir = fe4_mkdir,
	/*.chmod    = fe4_chmod,
	.chown    = fe4_chown,
	.truncate = fe4_truncate,*/
	.write    = fe4_write
};

static void show_help(const char *progname) {
	printf("usage: %s [options] <mountpoint>\n\n", progname);
	printf("File-system specific options:\n"
	       "    -h --help\n"
	       "\n");
}

int main(int argc, char *argv[]) {
	init_inodes();


	int ret;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	/* Parse options */
	if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
		return 1;

	/* When --help is specified, first print our own file-system
	   specific help text, then signal fuse_main to show
	   additional help (by adding `--help` to the options again)
	   without usage: line (by setting argv[0] to the empty
	   string) */
	if (options.show_help) {
		show_help(argv[0]);
		assert(fuse_opt_add_arg(&args, "--help") == 0);
		args.argv[0][0] = '\0';
	}

	assert(fuse_opt_add_arg(&args, "-f") == 0);

	ret = fuse_main(args.argc, args.argv, &fe4_oper, NULL);
	fuse_opt_free_args(&args);
	return ret;
}
