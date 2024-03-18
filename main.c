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

static int fe4_mkdir(const char *path, mode_t mode) {
    fuse_log(FUSE_LOG_INFO, "\033[1;35mfe4_mkdir started at path %s\n\033[m", path);

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

    fe4_inode *child = get_new_dir_inode(parent->stat.st_ino);

    char *path_for_basename = malloc(strlen(path) + 1);
    strcpy(path_for_basename, path);
    fe4_dirent de = {.inode_number = child->stat.st_ino};
    strcpy(de.filename, basename(path_for_basename));
    free(path_for_basename);

    add_dirent_to_inode(parent, &de);

    return 0;
}

static int fe4_mknod(const char *path, mode_t mode, dev_t dev) {
    fuse_log(FUSE_LOG_INFO, "\033[1;35mfe4_mknod started at path %s\n\033[m", path);

	if (!S_ISREG(mode)) {
		//fuse_log(FUSE_LOG_INFO, "Unsupported type requested in mknod\n");
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

    add_dirent_to_inode(parent, &de);

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

static int fe4_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    fuse_log(FUSE_LOG_INFO, "\033[1;35mfe4_getattr (stat) started at path %s\n\033[m", path);

	(void) fi;

	fe4_inode *in = get_inode_from_path(path);

	if (in == NULL)
		return -ENOENT;

	memcpy(stbuf, &in->stat, sizeof(struct stat));

	return 0;
}

static int fe4_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi,
			 enum fuse_readdir_flags flags) {
    fuse_log(FUSE_LOG_INFO, "\033[1;35mfe4_readdir started at path %s\n\033[m", path);

	(void) offset;
	(void) fi;
	(void) flags;

	fe4_inode *in = get_inode_from_path(path);

	if (in == NULL)
		return -ENOENT;

	if (!S_ISDIR(in->stat.st_mode))
		return -ENOENT;
		
	for (int i = 0; i < get_nb_children(in); i++) {
		const fe4_dirent *de = get_dirent_at(in, i);

        if (strcmp(de->filename, "/") == 0)
            continue;

		filler(buf, de->filename, NULL, 0, 0);
	}

	//fuse_log(FUSE_LOG_INFO, "readdir requested %s: %s\n", path, node_to_string(ret));

	return 0;
}

static int fe4_open(const char *path, struct fuse_file_info *fi) {
    fuse_log(FUSE_LOG_INFO, "\033[1;35mfe4_open started at path %s\n\033[m", path);

	fe4_inode *in = get_inode_from_path(path);

	if (in == NULL)
		return -ENOENT;

	if (!S_ISREG(in->stat.st_mode))
		return -ENOENT;

//	if ((fi->flags & O_ACCMODE) != O_RDONLY)
//		return -EACCES;

    if (fi->flags & O_TRUNC)
        in->stat.st_size = 0;

    return 0;
}

static int fe4_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    fuse_log(FUSE_LOG_INFO, "\033[1;35mfe4_read started at path %s\n\033[m", path);

	fe4_inode *in = get_inode_from_path(path);

	if (in == NULL)
		return -ENOENT;

	if (!S_ISREG(in->stat.st_mode))
		return -ENOENT;

	return read_inode(in, buf, size, offset);
}

static int fe4_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    fuse_log(FUSE_LOG_INFO, "\033[1;35mfe4_write started at path %s\n\033[m", path);

    fe4_inode *in = get_inode_from_path(path);

    if (in == NULL)
        return -ENOENT;

    if (!S_ISREG(in->stat.st_mode))
        return -ENOENT;

    return write_inode(in, buf, size, offset);
}

static int fe4_truncate(const char *path, off_t size, struct fuse_file_info *fi) { // TODO Put zeroes
    fuse_log(FUSE_LOG_INFO, "\033[1;35mfe4_truncate started at path %s\n\033[m", path);

	fe4_inode *in = get_inode_from_path(path);

    if (in == NULL)
        return -ENOENT;

    in->stat.st_size = size;

    return 0;
}

static int fe4_unlink(const char *path) {
    fuse_log(FUSE_LOG_INFO, "\033[1;35mfe4_unlink started at path %s\n\033[m", path);

    fe4_inode *in = get_inode_from_path(path);

    if (in == NULL)
        return -ENOENT;

    if (!S_ISREG(in->stat.st_mode))
        return -EISDIR;

    char *dirnm = dirname2(path);
    char *basenm = basename2(path);

    fe4_inode *parent = get_inode_from_path(dirnm);

    if (parent == NULL)
        return -ENOENT;

    if (!S_ISDIR(parent->stat.st_mode))
        return -ENOTDIR;

    delete_inode_at(in->stat.st_ino);

    for (int i = 0; i < get_nb_children(parent); i++) {
        fe4_dirent *de = get_dirent_at(parent, i);

        if (strcmp(de->filename, "/") == 0)
            continue;

        if (strcmp(de->filename, basenm) == 0) {
            delete_dirent_at(parent, i);

            free(dirnm);
            free(basenm);
            return 0;
        }
    }

    free(dirnm);
    free(basenm);
    return -EBUSY; // To identify this error
}

static int fe4_rmdir(const char *path) {
    fuse_log(FUSE_LOG_INFO, "\033[1;35mfe4_rmdir started at path %s\n\033[m", path);

    fe4_inode *in = get_inode_from_path(path);

    if (in == NULL)
        return -ENOENT;

    if (!S_ISDIR(in->stat.st_mode))
        return -ENOENT;

    if (get_nb_children(in) > 2)
        return -ENOTEMPTY;

    char *dirnm = dirname2(path);
    char *basenm = basename2(path);

    fe4_inode *parent = get_inode_from_path(dirnm);

    if (parent == NULL)
        return -ENOENT;

    if (!S_ISDIR(parent->stat.st_mode))
        return -ENOTDIR;

    delete_inode_at(in->stat.st_ino);

    for (int i = 0; i < get_nb_children(parent); i++) {
        fe4_dirent *de = get_dirent_at(parent, i);

        if (strcmp(de->filename, "/") == 0)
            continue;

        if (strcmp(de->filename, basenm) == 0) {
            delete_dirent_at(parent, i);

            free(dirnm);
            free(basenm);
            return 0;
        }
    }

    free(dirnm);
    free(basenm);
    return -EBUSY; // To identify this error
}

static int fe4_rename(const char *from, const char *to, unsigned int flags) {
    fuse_log(FUSE_LOG_INFO, "\033[1;35mfe4_rename started from %s to %s\n\033[m", from, to);

    fe4_inode *in = get_inode_from_path(from);

    if (in == NULL)
        return -ENOENT;

    char *dirnm = dirname2(from);
    char *basenm = basename2(from);

    fe4_inode *parent = get_inode_from_path(dirnm);

    if (parent == NULL)
        return -ENOENT;

    if (!S_ISDIR(parent->stat.st_mode))
        return -ENOTDIR;

    for (int i = 0; i < get_nb_children(parent); i++) {
        fe4_dirent *de = get_dirent_at(parent, i);

        if (strcmp(de->filename, "/") == 0)
            continue;

        if (strcmp(de->filename, basenm) == 0) {
            delete_dirent_at(parent, i);
            break;
        }
    }

    char *dirnm2 = dirname2(to);
    char *basenm2 = basename2(to);

    fe4_dirent new_dirent = {.inode_number = in->stat.st_ino};
    strcpy(new_dirent.filename, basenm2);

    fe4_inode *parent2 = get_inode_from_path(dirnm2); // TODO Vérifier que parent2 existe bien avant cette ligne

    add_dirent_to_inode(parent2, &new_dirent);

    return 0;
}

static const struct fuse_operations fe4_oper = {
	.init           = fe4_init,
	.getattr	= fe4_getattr,
	.readdir	= fe4_readdir,
	.open		  = fe4_open,
	.read		  = fe4_read,
    .mknod    = fe4_mknod,
    .mkdir = fe4_mkdir,
	.truncate = fe4_truncate,
	.write    = fe4_write,

    .unlink = fe4_unlink,
    .rmdir = fe4_rmdir,
    .rename = fe4_rename,
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
