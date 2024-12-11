#ifndef READ_TREE_H_
#define READ_TREE_H_

#define MAX_STRING_SIZE 128

#include "my_tree.h"

int get_func_num(char* input);
node_t* fill_node(char * buffer, size_t* position, my_tree_t* tree, node_t* parent);
my_tree_t make_prog_tree(char *buffer);
err_code_t fill_buffer(char **buffer_to_fill, const char* filename);
size_t get_file_len(const char *filename);

#endif // READ_TREE_H_
