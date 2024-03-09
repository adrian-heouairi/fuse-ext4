#ifndef DATA_STRUCTURES
#define DATA_STRUCTURES
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define ROOT_INODE 0

// An inode knows its own inode number (index) because of stat.st_ino
// Doesn't contain filename
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

// These functions return pointers to an fe4_inode or fe4_dirent which you can modify to directly affect the filesystem

void init_inodes(void);

fe4_inode *get_new_inode(mode_t type);

ssize_t read_inode(const fe4_inode *inode, void *buf, size_t count, off_t offset);

fe4_inode *get_inode_from_path(const char *path);

int get_nb_children(const fe4_inode *inode);

fe4_dirent *get_dirent_at(fe4_inode *parent, int index);

fe4_inode *get_inode_at(ino_t index);

#endif // !DATA_STRUCTURES
