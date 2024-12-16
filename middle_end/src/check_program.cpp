#include "check_program.h"

err_code_t check_program(my_tree_t* prog, nametable_t nametable)
{
    err_code_t err_code = OK;
    if ((err_code = check_definitions(prog, nametable)) != OK) return err_code;
    if ((err_code = check_params_num (prog, nametable)) != OK) return err_code;

    return err_code;
}

node_t* find_id_decl(my_tree_t* tree, node_t* node, char* func_name, bool is_var)
{
    node_t* to_ret = NULL;
    if (!is_var && node->type == STATEMENT && (int) node->data == FUNC_DECL && !strcmp(func_name, *(char**)&node->left->left->data))
    {
        return node;
    }
    if (is_var && node->type == VAR && !strcmp(func_name, *(char**)&node->data))
    {
        return node;
    }
    if (node->left  != NULL && (to_ret = find_id_decl(tree, node->left , func_name, is_var)) != NULL) return to_ret;
    if (node->right != NULL && (to_ret = find_id_decl(tree, node->right, func_name, is_var)) != NULL) return to_ret;

    return NULL;
}

err_code_t check_definitions(my_tree_t* prog, nametable_t nametable)
{
    for (size_t i = 0; i < MAX_ID_COUNT; i++)
    {
        identificator id = nametable[i];
        if (id.name[0] == '\0') break;

        if (id.type == FUNC_TYPE && id.is_defined == false)
        {
            if (find_id_decl(prog, prog->root, id.name, false) == NULL) // TODO: call this func once and check field is def
            {
                fprintf(stderr, "Function %s was not declared\n", id.name);
                abort();
            }
        }

        if (id.type == VAR_TYPE && id.dependence == MAX_ID_COUNT)
        {
            if (id.is_defined == false)
            {
                fprintf(stderr, "Var %s was not declared\n", id.name);
                abort();
            }
            node_t* def_node = find_id_decl(prog, prog->root, id.name, true);
            if (def_node->parent->left != def_node)
            {
                fprintf(stderr, "Var %s was not declared\n", id.name);
                abort();
            }
        }
    }

    return OK;
}

err_code_t find_func_def()
{
    return OK;
}

err_code_t check_params_num(my_tree_t* prog, nametable_t nametable)
{
    return OK;
}

