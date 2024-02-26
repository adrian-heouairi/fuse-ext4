#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>

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

	node *n = create_node(path, S_IFREG, 0777);
	// TODO: hanle errors e.g. user can not create file because of permissions or no space left on dev.
	add_child(root, n);

	fuse_log(FUSE_LOG_INFO, "mknod ended\n");

	return 0;
}*/

#define OPTION(t, p)                           \
    { t, offsetof(struct options, p), 1 }

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

	fe4_inode in; // We avoid managing memory ourselves that's why this is in the stack
	int r = get_inode_from_path(path, &in);

	//fuse_log(FUSE_LOG_INFO, "getattr requested %s: %\n", path, node_to_string(ret));

	if (r == -1)
		return -ENOENT;

	memcpy(stbuf, &in.stat, sizeof(struct stat));

	return 0;
}

static int fe4_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi,
			 enum fuse_readdir_flags flags) {
	(void) offset;
	(void) fi;
	(void) flags;

	fe4_inode in;
	int r = get_inode_from_path(path, &in);

	if (r == -1)
		return -ENOENT;

	if (!S_ISDIR(in.stat.st_mode))
		return -ENOENT;
		
	for (int i = 0; i < get_nb_children(&in); i++) {
		fe4_dirent de;
		get_dirent_at(&in, i, &de);

		filler(buf, de.filename, NULL, 0, 0);
	}

	//fuse_log(FUSE_LOG_INFO, "readdir requested %s: %s\n", path, node_to_string(ret));

	return 0;
}

static int fe4_open(const char *path, struct fuse_file_info *fi) {
	fe4_inode in;
	int r = get_inode_from_path(path, &in);

	if (r == -1)
		return -ENOENT;

	if (!S_ISREG(in.stat.st_mode))
		return -ENOENT;

	if ((fi->flags & O_ACCMODE) != O_RDONLY)
		return -EACCES;

	return 0;
}

/*static int fe4_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi) {
	size_t len;
	(void) fi;
	if(strcmp(path+1, "hello") != 0)
		return -ENOENT;

	len = strlen("h");
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, "h" + offset, size);
	} else
		size = 0;

	return size;
}*/

static int fe4_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi) {
	fe4_inode in;
	int r = get_inode_from_path(path, &in);

	if (r == -1)
		return -ENOENT;

	if (!S_ISREG(in.stat.st_mode))
		return -ENOENT;

	return read_inode(&in, buf, size, offset);
}

static const struct fuse_operations fe4_oper = {
	.init           = fe4_init,
	.getattr	= fe4_getattr,
	.readdir	= fe4_readdir,
	.open		  = fe4_open,
	.read		  = fe4_read,
  //.mknod    = fe4_mknod
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
