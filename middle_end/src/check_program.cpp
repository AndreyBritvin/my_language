#include "check_program.h"
#include <assert.h>

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

size_t count_parametrs(node_t* node)
{
    assert(node->type == STATEMENT && ((int) node->data == SEPARATOR));
    static size_t counter = 0;
    if (node->parent->type == STATEMENT && (int) node->parent->data == FUNC_SPEC)
    {
        counter = 0;
    }
    if (node->left  != NULL) counter++;
    if (node->right != NULL) count_parametrs(node->right);
    return counter;
}

err_code_t check_all_calls(my_tree_t* prog, node_t* node, size_t params_amount)
{
    if (node->type == STATEMENT && (int) node->data == FUNC_SPEC)
    {
        size_t func_params = count_parametrs(node->right);
        if (func_params != params_amount)
        {
            fprintf(stderr, "In func %s should be %zu params, but %zu instead\n", *(char**)&node->left->data, params_amount, func_params);
            abort();
        }
    }

    if (node->left  != NULL) check_all_calls(prog, node->left,  params_amount);
    if (node->right != NULL) check_all_calls(prog, node->right, params_amount);

    return OK;
}

err_code_t check_params_num(my_tree_t* prog, nametable_t nametable)
{
    for (size_t i = 0; i < MAX_ID_COUNT; i++)
    {
        identificator id = nametable[i];
        if (id.name[0] == '\0') break;

        if (id.type == FUNC_TYPE)
        {
            size_t def_params = count_parametrs(find_id_decl(prog, prog->root, id.name, false)->left->right);
            check_all_calls(prog, prog->root, def_params);
        }
    }

    return OK;
}

