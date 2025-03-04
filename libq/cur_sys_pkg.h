#ifndef CUR_SYS_PKG
#define CUR_SYS_PKG

//BST node with an entry for key(based on hash of filename) hash found in /var/db/pkg/.../CONTENTS, package fullname of the package
typedef struct cur_pkg_tree_node cur_pkg_tree_node;

void in_order_visit(cur_pkg_tree_node *root);
//read file from a CONTENTS and create a tree with it's field
void read_file_add_data(void *root_to_conv);
//return 1 if the file B of the package C is has the same hash of the one in /var/db/pkg/.../CONTENTS
int is_default(cur_pkg_tree_node *root,char *file_path_complete);
//dealloc the tree
void destroy_cur_pkg_tree(cur_pkg_tree_node **root);

#endif // !CUR_SYS_PKG
