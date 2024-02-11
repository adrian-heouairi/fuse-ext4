#ifndef DATA_STRUCTURES
#define DATA_STRUCTURES
#include <sys/stat.h>

// children is a Flexible Array Member
typedef struct node node;
struct node {
  char path[2048];
  struct stat info;
  int nb_children;
  int max_nb_children;
  node *children[];
};

node *create_node(const char *path, int file_type, int permissions);

void add_child(node *parent, node *child);

node *get_node_from_path(const char *path, node *current);

const char *node_to_string(const node *n);








typedef struct fe4_inode fe4_inode;
struct fe4_inode {
  struct stat stat;
  char contents[16384];
};

typedef struct fe4_dirent fe4_dirent;
struct fe4_dirent {
  char filename[256];
  int inode_number;
};

ssize_t fe4_read_inode(fe4_inode *inode, void *buf, size_t count);

ssize_t fe4_get_dirents(fe4_inode *inode, void *buf, size_t count);

#endif // !DATA_STRUCTURES
