#include "recursive_descent_parser.h"
#include <stdio.h>
#include <math.h>
#include "my_tree.h"
#include <string.h>
#include <assert.h>
#include "op_types.h"
// const char* input = "25*10*(3*(25-10*2)+1)-15$";
// const char* input = "25+10-10*3$";
// int pos = 0;

#define SYNTAX_ERROR(...) fprintf(stderr, __VA_ARGS__);

my_tree_t make_tree(char *buffer)
{
    assert(buffer);
    tokens* programm_tokens = (tokens*) calloc(MAXIMUM_LEXEMS_COUNT, sizeof(tokens));

    size_t tokens_num = lexical_analysis(programm_tokens, buffer);

    printf_tokens(programm_tokens, tokens_num);

    my_tree_t tree_to_fill = get_grammatic(programm_tokens);
    TREE_DUMP(&tree_to_fill, tree_to_fill.root, "I am gROOT (generated this tree after reading file)");

    free_tokens(programm_tokens, tokens_num);
    return tree_to_fill;
}

my_tree_t get_grammatic(tokens* input)
{
    INIT_TREE(tree_to_ret);
    free(tree_to_ret.root);
    size_t pos = 0;
    tree_to_ret.root = get_expression(&tree_to_ret, input, &pos);
    if (input[pos].type != END) SYNTAX_ERROR("At pos = %d expected $, but %c instead\n", pos, input[pos].value);

    return tree_to_ret;
}

node_t* get_func(my_tree_t* tree, tokens* input, size_t* pos)
{
    int func = (int) input[*pos].value;
    (*pos)++;
    node_t* left_subtree = get_primary(tree, input, pos);
    node_t* to_ret = new_node(tree, OP, func, left_subtree, NULL);
    left_subtree->parent = to_ret;

    return to_ret;
}

node_t* get_exp(my_tree_t* tree, tokens* input, size_t* pos)
{
    node_t* val = get_primary(tree, input, pos);
    while ((int) input[*pos].value == EXP)
    {
        (*pos)++;
        node_t* val_2 = get_primary(tree, input, pos);
        val = new_node(tree, OP, EXP, val, val_2);
        val->left->parent = val->right->parent = val;
    }

    return val;
}

node_t* get_mul_div(my_tree_t* tree, tokens* input, size_t* pos)
{
    node_t* val = get_exp(tree, input, pos);
    while ((int) input[*pos].value == MUL || (int) input[*pos].value == DIV)
    {
        int operation = (int) input[*pos].value;
        (*pos)++;
        node_t* val_2 = get_exp(tree, input, pos);
        if (operation == MUL)
        {
            val = new_node(tree, OP, MUL, val, val_2);
            val->left->parent = val->right->parent = val;
        }
        if (operation == DIV)
        {
            val = new_node(tree, OP, DIV, val, val_2);
            val->left->parent = val->right->parent = val;
        }
    }

    return val;
}

node_t* get_primary(my_tree_t* tree, tokens* input, size_t* pos)
{
    printf("Token type = %d\n", input[*pos].type);
    if ((int) input[*pos].value == BRACKET_OPEN)
    {
        (*pos)++;
        node_t* val = get_expression(tree, input, pos);
        if ((int) input[*pos].value != BRACKET_CLOS) SYNTAX_ERROR("At pos = %d expected ), but %c instead\n", pos, input[*pos]);
        (*pos)++;
        return val;
    }
    if ((int) input[*pos].type == VAR) return get_variable(tree, input, pos);
    if ((int) input[*pos].type == NUM) return get_number  (tree, input, pos);
    if ((int) input[*pos].type == OP ) return get_func    (tree, input, pos);
    // to_ret = get_number(tree, input, pos);

    return NULL;
}

node_t* get_expression(my_tree_t* tree, tokens* input, size_t* pos)
{
    node_t* val = get_mul_div(tree, input, pos);
    while ((int) input[*pos].value == ADD || (int) input[*pos].value == SUB)
    {
        int operation = (int) input[*pos].value;
        (*pos)++;
        node_t* val_2 = get_mul_div(tree, input, pos);
        if (operation == ADD)
        {
            val = new_node(tree, OP, ADD, val, val_2);
            val->left->parent = val->right->parent = val;
        }
        if (operation == SUB)
        {
            val = new_node(tree, OP, SUB, val, val_2);
            val->left->parent = val->right->parent = val;
        }
    }

    return val;
}

node_t* get_number(my_tree_t* tree, tokens* input, size_t* pos)
{
    tree_val_t val = input[*pos].value;

    (*pos)++;

    return new_node(tree, NUM, val, NULL, NULL);
}

node_t* get_variable(my_tree_t* tree, tokens* input, size_t* pos)
{
    if (input[*pos].type != VAR) return NULL;

    printf("Well, we are in get_variable. Type = %d, addr in value is %p and str is \n",
             input[*pos].type, *(char**)&input[*pos].value);
    if (strcmp(*(char**)&input[*pos].value, "x") == 0)
    {
        (*pos)++;
        return new_node(tree, VAR, 'x', NULL, NULL);
    }
    else
    {
        printf("This var is not available\n");
    }

    return NULL;
}
