#ifndef MAKE_ASSEMBLER_FILE_H_
#define MAKE_ASSEMBLER_FILE_H_

#include "my_tree.h"
#include "assert.h"
#include "name_table.h"

enum var_writing
{
    VAR_NAME,
    VAR_POP,
    VAR_PUSH,
};

err_code_t print_tabs(FILE* output, size_t recurs_level);
size_t get_num_of_global_vars();
size_t get_amount_of_local_vars_in_func(size_t func_num);

err_code_t generate_assembler(my_tree_t* tree, const char* filename, nametable_t nt);
err_code_t write_to_assembler(FILE* output, my_tree_t* tree, node_t* curr_node, size_t recurs_level);
err_code_t write_expression(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_equal(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_var(FILE* output, my_tree_t* tree, node_t* node, var_writing is_push);
err_code_t write_print(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_if(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level);
err_code_t write_while(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level);
err_code_t write_if_operator(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_return(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_func_call(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_func_decl(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level);
err_code_t write_parametrs(FILE* output, my_tree_t* tree, node_t* node, int    recurs_level, bool is_memory);

#endif // MAKE_ASSEMBLER_FILE_H_
