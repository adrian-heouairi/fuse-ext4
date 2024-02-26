#ifndef DATA_STRUCTURES
#define DATA_STRUCTURES
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define ROOT_INODE 0

typedef struct fe4_inode fe4_inode;
struct fe4_inode {
  struct stat stat;
  char contents[16384]; // Should not be accessed directly
};

typedef struct fe4_dirent fe4_dirent;
struct fe4_dirent {
  char filename[256];
  ino_t inode_number;
};

void init_inodes();

ssize_t read_inode(const fe4_inode *inode, void *buf, size_t count, off_t offset);

// Return 0 on success, -1 on error
int get_inode_from_path(const char *path, fe4_inode *ret);

int get_nb_children(const fe4_inode *inode);

int get_dirent_at(const fe4_inode *parent, int index, fe4_dirent *ret);

int get_inode_at(int index, fe4_inode *ret);

#endif // !DATA_STRUCTURES
