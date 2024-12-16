#include "name_table.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

err_code_t create_name_table(identificator** name_table)
{
    SAFE_CALLOC(temp_nt, identificator, MAX_ID_COUNT);

    *name_table = temp_nt;
    for (size_t i = 0; i < MAX_ID_COUNT; i++)
    {
        temp_nt[i].dependence = MAX_ID_COUNT;
    }

    return OK;
}

err_code_t delete_name_table(identificator* name_table)
{
    free(name_table);

    return OK;
}

err_code_t print_name_table(identificator* name_table)
{
    for (size_t i = 0; i < MAX_ID_COUNT; i++)
    {
        identificator id = name_table[i];
        printf("Name = %-10.10s type = %-4s len = %3zu dep = %3zu is_def = %1d full_ind = %2d dep_node = %p\n",
                id.name,   id.type == VAR_TYPE ? "VAR": id.type == FUNC_TYPE ? "FUNC" : "PARM",
                id.length, id.dependence, id.is_defined, id.full_index, id.node_dep);
    }

    return OK;
}

err_code_t add_element(nametable_t nt, identificator id)
{
    static size_t var_index = 0;
    static size_t inside_func_index = 0;
    if (id.type == FUNC_TYPE) inside_func_index = 0;
    static size_t index = 0; // TODO: make universal for different nametables. Now it correctly fill only one table
    if ((id.type == VAR_TYPE || id.type == PARAM_TYPE) && id.node_dep == NULL) id.full_index = var_index++;
    if ((id.type == VAR_TYPE || id.type == PARAM_TYPE) && id.node_dep != NULL) id.full_index = inside_func_index++;
    nt[index++] = id;

    return OK;
}

size_t get_element_index(nametable_t nt, char* elem)
{
    for (size_t i = 0; i < MAX_ID_COUNT; i++)
    {
        if (!strcmp(nt[i].name, elem))
        {
            return i;
        }
    }

    return MAX_ID_COUNT;
}

size_t get_num_of_global_vars(nametable_t nametable)
{
    size_t max_index = 0;
    for (size_t i = 0; i < MAX_ID_COUNT; i++)
    {
        if (nametable[i].full_index > max_index && nametable[i].dependence == MAX_ID_COUNT)
        {
            max_index = nametable[i].full_index;
        }
    }

    return max_index > 0 ? max_index + 1 : 0;
}

size_t get_amount_of_local_vars_in_func(size_t func_num, nametable_t nametable)
{
    assert(nametable[func_num].type == FUNC_TYPE);

    for (size_t i = func_num + 1; i < MAX_ID_COUNT; i++)
    {
        if (nametable[i].dependence != func_num
         || nametable[i].type       == FUNC_TYPE
         || nametable[i].name[0]    == '\0')
        {
            return i - func_num - 1; // amount means 1,2,3,4...
        }
    }

    return 0;
}
// TODO: think about merging up and down function
size_t get_amount_of_parametrs(size_t func_num, nametable_t nametable)
{
    assert(nametable[func_num].type == FUNC_TYPE);

    for (size_t i = func_num + 1; i < MAX_ID_COUNT; i++)
    {
        if (nametable[i].dependence != func_num
         || nametable[i].type       != PARAM_TYPE
         || nametable[i].name[0]    == '\0')
        {
            return i - func_num - 1; // amount means 1,2,3,4...
        }
    }
}

#define FUNC_NAME_BY_NODE(node) *(char**)&node->left->left->data

size_t get_current_func(node_t* node, nametable_t nametable)
{
    if (node->type == STATEMENT && (int) node->data == FUNC_DECL)
    {
        return get_element_index(nametable, FUNC_NAME_BY_NODE(node));
    }
    if (node->parent != NULL) return get_current_func(node->parent, nametable);

    return MAX_ID_COUNT;
}
