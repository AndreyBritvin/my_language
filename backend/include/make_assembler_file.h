#ifndef MAKE_ASSEMBLER_FILE_H_
#define MAKE_ASSEMBLER_FILE_H_

#include "my_tree.h"
#include "assert.h"
#include "name_table.h"

typedef err_code_t (write_op_t)
                (FILE* output, my_tree_t* tree, node_t* node,                      nametable_t nametable);
typedef err_code_t (write_op_with_recurs_t)
                (FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level, nametable_t nametable);

enum var_writing
{
    VAR_NAME,
    VAR_POP,
    VAR_PUSH,
};

err_code_t print_tabs(FILE* output, size_t recurs_level);

err_code_t             generate_assembler(my_tree_t* tree, const char* filename, nametable_t nt);
write_op_with_recurs_t write_to_assembler;
err_code_t             write_expression(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable);
write_op_t             write_equal;
err_code_t             write_var(FILE* output, my_tree_t* tree, node_t* node, var_writing is_push, nametable_t nametable);
write_op_t             write_print;
write_op_with_recurs_t write_if;
write_op_with_recurs_t write_while;
write_op_t             write_if_operator;
write_op_t             write_return;
write_op_t             write_func_call;
write_op_with_recurs_t write_func_decl;
err_code_t             write_parametrs(FILE* output, my_tree_t* tree, node_t* node, int recurs_level, bool is_memory, nametable_t nametable);
err_code_t align_rsp(FILE* output);

#endif // MAKE_ASSEMBLER_FILE_H_
