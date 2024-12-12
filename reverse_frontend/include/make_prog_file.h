#ifndef MAKE_PROG_FILE_H_
#define MAKE_PROG_FILE_H_

#include "my_tree.h"
#include "assert.h"


err_code_t generate_prog_file(my_tree_t* tree, const char* filename);
err_code_t write_to_file(FILE* output, my_tree_t* tree, node_t* curr_node, size_t recurs_level);
err_code_t write_expression(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_equal(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_var(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_print(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_if(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level);
err_code_t write_while(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level);
err_code_t print_tabs(FILE* output, size_t recurs_level);
err_code_t write_func_decl(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level);
err_code_t write_func_call(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_return(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_args(FILE* output, my_tree_t* tree, node_t* node);

#endif // MAKE_PROG_FILE_H_
