#include "my_tree.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "my_log.h"

node_t* new_node(my_tree_t* tree, op_type_t type, tree_val_t data, node_t* left_node, node_t* right_node)
{
    assert(tree);

    node_t* node = (node_t*) calloc(1, sizeof(node_t));
    node->type  =       type;
    node->data  =       data;
    node->left  =  left_node;
    node->right = right_node;

    tree->size += 1;

    return node;
}

err_code_t tree_ctor(my_tree_t* tree)
{
    tree->size = 1;
    // tree->root = new_node(tree, 0, 0, NULL, NULL);

    return OK;
}

err_code_t tree_dtor(my_tree_t* tree)
{
    CHECK_TREE(tree);
    node_dtor(tree->root);

    return OK;
}

err_code_t node_dtor(node_t* node)
{
    assert(node);
    if (node->type == VAR) free(*(char**)&node->data);

    if (node->left  != NULL) node_dtor(node->left);
    if (node->right != NULL) node_dtor(node->right);

    free(node);

    return OK;
}

err_code_t print_tree(my_tree_t* tree)
{
    CHECK_TREE(tree);

    print_node(tree->root);

    return OK;
}

err_code_t print_node(node_t* tree)
{
    if (tree == NULL) return ERROR_TREE_IS_NULL;

    printf("(");
    if (tree->left  != NULL) print_node(tree->left);
    printf("%lg", tree->data);
    if (tree->right != NULL) print_node(tree->right);
    printf(")");

    return OK;
}

err_code_t verificator(my_tree_t* tree, node_t* node, size_t recurs_level)
{
    assert(tree);

    // size_t index_to_append = 0;
    // printf("Tree size is %zd, curr recurs is %zd\n", tree->size, recurs_level);
    if (recurs_level > tree->size)
    {
        return ERROR_TREE_LOOPED;
    }

    if (node->left  != NULL && node->left ->parent != node) return ERROR_PARENT_DONT_MATCH;
    if (node->right != NULL && node->right->parent != node) return ERROR_PARENT_DONT_MATCH;

    err_code_t err_code = OK;
    if (node->left  != NULL)                   err_code = verificator(tree, node->left,  recurs_level + 1);
    if (node->right != NULL && err_code == OK) err_code = verificator(tree, node->right, recurs_level + 1);

    return err_code;
}

node_t* copy_subtree(my_tree_t* main_tree, node_t* node)
{
    assert(main_tree);
    assert(node);

    node_t* copy_node = new_node(main_tree, node->type, node->data, NULL, NULL);

    node_t* left_sub  = NULL;
    node_t* right_sub = NULL;

    if (node->left  != NULL)
    {
        left_sub  = copy_subtree(main_tree, node->left);
        left_sub->parent = copy_node;
    }
    if (node->right != NULL)
    {
        right_sub = copy_subtree(main_tree, node->right);
        right_sub->parent = copy_node;
    }

    copy_node->left  = left_sub;
    copy_node->right = right_sub;

    return copy_node;
}

int subtree_var_count(my_tree_t* tree, node_t* node)
{
    if (node->type == VAR) return ONE_VAR;
    if (node->left  != NULL && subtree_var_count(tree, node->left)  == ONE_VAR) return ONE_VAR;
    if (node->right != NULL && subtree_var_count(tree, node->right) == ONE_VAR) return ONE_VAR;

    return ZERO_VAR;
}

err_code_t generate_subtrees(my_tree_t* tree, node_t* curr_node, size_t recurs_level)
{
    size_t curr_level = (recurs_level + 1) % SUBTREE_DEPTH;
    static char subtree_name = 'A';
    static size_t index = 0;

    if (recurs_level == 0)
    {
        index = 0;
        subtree_name = 'A';
    }

    if (curr_level == 0 && (curr_node->left  != NULL && curr_node->left ->type == OP &&
                            curr_node->right != NULL && curr_node->right->type == OP ))
    {
        if (subtree_name >= 'X')
        {
            subtree_name = 'A';
        }

        if (curr_node->left != NULL)
        {
            char* index_string = (char*) calloc(8, sizeof(char));
            sprintf(index_string, "%c%zu", subtree_name++, index++);
            node_t* replaced_subtree_left  = new_node(tree, SUBTREE, 0, curr_node->left, NULL);
            memcpy(&replaced_subtree_left->data, index_string, sizeof(char*));
            replaced_subtree_left->parent =  curr_node;
            curr_node->left->parent = replaced_subtree_left;
            curr_node->left         = replaced_subtree_left;
            free(index_string);
        }
        if (curr_node->right != NULL)
        {
            char* index_string = (char*) calloc(8, sizeof(char));
            sprintf(index_string, "%c%zu", subtree_name++, index++);
            node_t* replaced_subtree_right = new_node(tree, SUBTREE, 0, curr_node->right, NULL);
            memcpy(&replaced_subtree_right->data, index_string, sizeof(char*));
            replaced_subtree_right->parent = curr_node;
            curr_node->right->parent = replaced_subtree_right;
            curr_node->right         = replaced_subtree_right;
            free(index_string);
        }
    }

    if (curr_node->type == SUBTREE) generate_subtrees(tree, curr_node->left,  recurs_level + 1);
    if (curr_node->type != SUBTREE && curr_node->left  != NULL)
                                    generate_subtrees(tree, curr_node->left,  recurs_level + 1);
    if (curr_node->type != SUBTREE && curr_node->right != NULL)
                                    generate_subtrees(tree, curr_node->right, recurs_level + 1);

    return OK;
}

err_code_t remove_subtrees(my_tree_t* tree, node_t* curr_node)
{
    if (curr_node->left  != NULL) remove_subtrees(tree, curr_node->left);
    if (curr_node->right != NULL) remove_subtrees(tree, curr_node->right);

    if (curr_node->type == SUBTREE)
    {
        // printf("Node at %p\n", curr_node);
        // printf("node addre to delete %p with name %s\n", &curr_node->data, (char*) &curr_node->data);
        // char* to_delete = NULL;
        // memcpy(&to_delete, &(curr_node->data), sizeof(char*));
        // printf("Addr_to_del = %p\n", &to_delete);
        // printf("Str to delete = %s\n", &to_delete);
        // free(&to_delete);

        if (curr_node->parent->left  == curr_node)
        {
            curr_node->parent->left = curr_node->left;
            curr_node->left->parent = curr_node->parent;
        }
        if (curr_node->parent->right == curr_node)
        {
            curr_node->parent->right = curr_node->left;
            curr_node->left->parent  = curr_node->parent;
        }

        free(curr_node);
    }

    return OK;
}

err_code_t overwrite_file(my_tree_t* tree, const char* filename)
{
    assert(tree);
    assert(filename);

    FILE * SAFE_OPEN_FILE(overwrite_file, filename, "w");

    fprintf(overwrite_file, "434-format v1.0\n");
    write_node(tree, tree->root, 0, overwrite_file);

    fclose(overwrite_file);

    return OK;
}

err_code_t write_node(my_tree_t* tree, node_t* node, size_t recurs_level, FILE* overwrite_file)
{
    assert(tree);
    assert(node);
    assert(overwrite_file);

    print_n_spaces(recurs_level * 4, overwrite_file);
    if (node->type == NUM)
    {
        fprintf(overwrite_file, "{\"%lg\"", node->data);
    }
    else if (node->type == OP || node->type == STATEMENT)
    {
        fprintf(overwrite_file, "{\"%s\"", all_ops[(int) node->data].standart_text);
    }
    else if (node->type == VAR)
    {
        fprintf(overwrite_file, "{\"%s\"", *(char**)&node->data);
    }

    if (node->left != NULL)
    {
        fprintf(overwrite_file, "\n");
        write_node(tree, node->left, recurs_level + 1, overwrite_file);
    }
    if (node->right != NULL)
    {
        write_node(tree, node->right, recurs_level + 1, overwrite_file);
    }
    if (node->right == NULL && node->left == NULL)
    {
        fprintf(overwrite_file, "}\n");
    }
    else
    {
        print_n_spaces(recurs_level * 4, overwrite_file);
        fprintf(overwrite_file, "}\n");
    }

    return OK;
}

err_code_t print_n_spaces(size_t num, FILE* where)
{
    assert(where);

    for (size_t i = 0; i < num; i++)
    {
        fprintf(where, " ");
    }

    return OK;
}
