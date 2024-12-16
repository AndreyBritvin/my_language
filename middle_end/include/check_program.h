#ifndef CHECK_PROGRAM_H_
#define CHECK_PROGRAM_H_

#include "my_tree.h"
#include "name_table.h"

err_code_t check_program(my_tree_t* prog, nametable_t nametable);
err_code_t check_definitions(my_tree_t* prog, nametable_t nametable);
err_code_t check_params_num(my_tree_t* prog, nametable_t nametable);

#endif // CHECK_PROGRAM_H_
