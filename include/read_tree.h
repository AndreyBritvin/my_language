#ifndef READ_TREE_H_
#define READ_TREE_H_

#include "name_table.h"
#include "my_tree.h"

int get_func_num(char* input);
node_t* fill_node(char * buffer, size_t* position, my_tree_t* tree, node_t* parent, nametable_t nametable);
my_tree_t make_prog_tree(char *buffer, nametable_t nametable);
err_code_t fill_buffer(char **buffer_to_fill, const char* filename);
size_t get_file_len(const char *filename);
size_t check_signature(char* input);
node_t* get_func_node_dependence(nametable_t nt, node_t* dep_node);
err_code_t get_func_dependence(nametable_t nt);

#endif // READ_TREE_H_
