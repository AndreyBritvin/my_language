#include "name_table.h"
#include <stdio.h>
#include <string.h>

err_code_t create_name_table(identificator** name_table)
{
    SAFE_CALLOC(temp_nt, identificator, MAX_ID_COUNT);

    *name_table = temp_nt;

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
        printf("Name = %-10s type = %-4s len = %3zu dep = %3zu is_def = %1d full_ind = %2d\n",
                id.name,   id.type == VAR_TYPE ? "VAR":"FUNC",
                id.length, id.dependence, id.is_defined, id.full_index);
    }

    return OK;
}

err_code_t add_element(nametable_t nt, identificator id)
{
    static size_t var_index = 0;
    static size_t inside_func_index = 0;
    if (id.type == FUNC_TYPE) inside_func_index = 0;
    static size_t index = 0; // TODO: make universal for different nametables. Now it correctly fill only one table
    if (id.type == VAR_TYPE && id.node_dep == NULL) id.full_index = var_index++;
    if (id.type == VAR_TYPE && id.node_dep != NULL) id.full_index = inside_func_index++;
    nt[index++] = id;

    return OK;
}

size_t is_element_in_nt(nametable_t nt, char* elem)
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
