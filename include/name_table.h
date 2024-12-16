#ifndef NAME_TABLE_H_
#define NAME_TABLE_H_

#include "utils.h"
#include "my_tree.h"
#define MAX_ID_COUNT 30

enum id_types
{
    VAR_TYPE,
    FUNC_TYPE,
    PARAM_TYPE,
    LOC_TYPE,
};

struct identificator
{
    char        name[MAX_STRING_SIZE];
    size_t      length;
    id_types    type;
    node_t*     node_dep;
    size_t      dependence;
    bool        is_defined;
    size_t      full_index;
};

typedef identificator* nametable_t;

err_code_t create_name_table(nametable_t* name_table);
err_code_t delete_name_table(nametable_t name_table);
err_code_t print_name_table(nametable_t name_table);
err_code_t add_element(nametable_t nt, identificator id);
size_t get_element_index(nametable_t nt, char* elem);

size_t get_num_of_global_vars(nametable_t nametable);
size_t get_amount_of_local_vars_in_func(size_t func_num, nametable_t nametable);
size_t get_amount_of_parametrs(size_t func_num, nametable_t nametable);
size_t get_current_func(node_t* node, nametable_t nametable);

#endif // NAME_TABLE_H_
