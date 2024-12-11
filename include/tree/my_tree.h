#ifndef MY_TREE_H_
#define MY_TREE_H_
#include <stdlib.h>
#include "utils.h"
#include "op_types.h"
#include <stdio.h>
#include <stdarg.h>

#ifndef NDEBUG
    #define CHECK_TREE(tree_to_check); \
        if (int err_num = verificator(tree_to_check, tree_to_check->root, 0)) return err_num;
    #define TREE_DUMP(tree_to_dump, curr_node, ...) \
        tree_dump(tree_to_dump, curr_node, __func__, __FILE__, __LINE__, __VA_ARGS__);
    #define DEBUG_INFO , const char * funcname, const char * filename, const int fileline
    #define INIT_TREE(tree_name) my_tree_t tree_name = {};  tree_ctor(&tree_name);                     \
                                                            tree_name.rootname = #tree_name;           \
                                                            tree_name.funcname = __func__;             \
                                                            tree_name.filename = __FILE__;             \
                                                            tree_name.codeline = __LINE__;
#else
    #define TREE_DUMP(tree_to_dump, curr_node, action) \
        tree_dump(tree_to_dump, curr_node, action);
    #define DEBUG_INFO
    #define INIT_TREE(tree_name) my_tree_t tree_name = {};  tree_ctor(&tree_name);
#endif // NDEBUG

const size_t SUBTREE_DEPTH = 4;

typedef double tree_val_t;
typedef int   op_type_t;

enum vars_count
{
    ZERO_VAR,
    ONE_VAR,
};

struct node_t
{
    op_type_t   type;
    tree_val_t  data;
    node_t *   right;
    node_t *    left;
    node_t *  parent;
};

struct my_tree_t
{
    node_t* root;
    size_t  size;
#ifndef NDEBUG
    const char * filename;
    const char * funcname;
    const char * rootname;
    int          codeline;
#endif
};

node_t* new_node(my_tree_t* tree, op_type_t type, tree_val_t data, node_t* left_node, node_t* right_node);

err_code_t tree_ctor(my_tree_t* tree);
err_code_t tree_dtor(my_tree_t* tree);
err_code_t node_dtor (node_t* tree);

err_code_t print_tree(my_tree_t* tree);
err_code_t print_node(node_t* tree);

char* give_op_type(node_t *node);
char* give_var_type(node_t *node);
char* give_num_type(node_t *node);
char* give_subtree_name(node_t* node);
char* get_data_field(node_t* node);
const char* type_to_str(node_t* node);

int subtree_var_count(my_tree_t* tree, node_t* node);
node_t* copy_subtree(my_tree_t* main_tree, node_t* node);

err_code_t paste_instruction();
err_code_t tree_dump(my_tree_t* tree, node_t* curr_node DEBUG_INFO, const char * curr_action, ...);
err_code_t verificator(my_tree_t* tree, node_t* node, size_t recurs_level);
err_code_t generate_subtrees(my_tree_t* tree, node_t* curr_node, size_t recurs_level);
err_code_t remove_subtrees(my_tree_t* tree, node_t* curr_node);

err_code_t overwrite_file(my_tree_t* tree, const char* filename);
err_code_t write_node(my_tree_t* tree, node_t* node, size_t recurs_level, FILE* overwrite_file);
err_code_t print_n_spaces(size_t num, FILE* where);

#endif // MY_TREE_H_
