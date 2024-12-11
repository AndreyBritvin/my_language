#ifndef MAKE_ASSEMBLER_FILE_H_
#define MAKE_ASSEMBLER_FILE_H_

#include "my_tree.h"
#include "assert.h"


err_code_t generate_assembler(my_tree_t* tree, const char* filename);
err_code_t write_to_assembler(FILE* output, my_tree_t* tree, node_t* curr_node);
err_code_t write_math_operation(FILE* output, my_tree_t* tree, node_t* node);
err_code_t write_equal(FILE* output, my_tree_t* tree, node_t* node);

#endif // MAKE_ASSEMBLER_FILE_H_
