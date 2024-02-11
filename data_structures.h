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

#endif // !DATA_STRUCTURES
