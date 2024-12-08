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
#define CURR_VAL  input[POS].value
#define CURR_TYPE input[POS].type
#define POS       (*pos)
#define INCR      (*pos)++

#define CUSTOM_SYNTAX_ERROR(...) {fprintf(stderr, __VA_ARGS__); abort();}
#define SYNTAX_ERROR(expected) {fprintf(stderr, "At line %zu column %zu expected %s, but %c instead\n",\
                                            input[*pos].line, input[*pos].column, expected, CURR_VAL); abort();}

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
    tree_to_ret.root = get_statement(&tree_to_ret, input, &pos);
    if ((int) input[pos].type != END) CUSTOM_SYNTAX_ERROR("At line %zu column %zu expected $ but %c instead\n",
                                                     input[pos].line, input[pos].column, (char) input[pos].value);

    return tree_to_ret;
}

node_t* get_func(my_tree_t* tree, tokens* input, size_t* pos)
{
    int func = (int) CURR_VAL;
    (*pos)++;
    if ((int) CURR_VAL != BRACKET_OPEN) SYNTAX_ERROR(all_ops[BRACKET_OPEN].text);
    INCR;

    node_t* left_subtree = get_expression(tree, input, pos);

    if ((int) CURR_VAL != BRACKET_CLOS) SYNTAX_ERROR(all_ops[BRACKET_CLOS].text);
    INCR;

    node_t* to_ret = new_node(tree, OP, func, left_subtree, NULL);
    left_subtree->parent = to_ret;

    return to_ret;
}

node_t* get_exp(my_tree_t* tree, tokens* input, size_t* pos)
{
    node_t* val = get_primary(tree, input, pos);
    while ((int) CURR_VAL == EXP)
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
    while ((int) CURR_VAL == MUL || (int) CURR_VAL == DIV)
    {
        int operation = (int) CURR_VAL;
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
    if (CURR_TYPE == OP && (int) CURR_VAL == BRACKET_OPEN)
    {
        (*pos)++;
        node_t* val = get_expression(tree, input, pos);
        if (CURR_TYPE != OP || (int) CURR_VAL != BRACKET_CLOS) SYNTAX_ERROR(all_ops[BRACKET_CLOS].text);
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
    while ((int) CURR_VAL == ADD || (int) CURR_VAL == SUB)
    {
        int operation = (int) CURR_VAL;
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
    tree_val_t val = CURR_VAL;

    (*pos)++;

    return new_node(tree, NUM, val, NULL, NULL);
}

node_t* get_variable(my_tree_t* tree, tokens* input, size_t* pos)
{
    if (input[*pos].type != VAR) return NULL;

    printf("Well, we are in get_variable. Type = %d, addr in value is %p and str is \n",
             input[*pos].type, *(char**)&CURR_VAL);
    if (strcmp(*(char**)&CURR_VAL, "x") == 0)
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

node_t* get_assingnment(my_tree_t* tree, tokens* input, size_t* pos)
{
    node_t* identificator = get_variable(tree, input, pos);
    if (identificator == NULL)
    {
        return NULL;
    }
    printf("In assignment\n");
    if (CURR_TYPE != STATEMENT || (int) CURR_VAL != EQUAL) SYNTAX_ERROR(all_ops[EQUAL].text);
    INCR;

    node_t* right_expr = get_expression(tree, input, pos);
    node_t* equal_node = new_node(tree, STATEMENT, EQUAL, identificator, right_expr);
    identificator->parent = right_expr->parent = equal_node;

    if (CURR_VAL != STATEMENT_END) SYNTAX_ERROR(all_ops[STATEMENT_END].text);
    INCR;

    return equal_node;
}

node_t* get_statement(my_tree_t* tree, tokens* input, size_t* pos)
{
    if (CURR_TYPE == STATEMENT && (int) CURR_VAL == SCOPE_OPEN)
    {
        INCR;
        node_t* inside = get_statement(tree, input, pos);
        if (CURR_TYPE != STATEMENT && (int) CURR_VAL != SCOPE_CLOS) SYNTAX_ERROR(all_ops[SCOPE_CLOS].text);
        INCR;
        return inside;
    }
    node_t* to_ret = NULL;
    if      ((to_ret = get_if_state   (tree, input, pos)) != NULL)  to_ret;
    else if ((to_ret = get_assingnment(tree, input, pos)) != NULL)  to_ret;

    if (to_ret == NULL) return NULL;

    node_t* another_node = get_statement(tree, input, pos);

    node_t* to_ret_2 = new_node(tree, STATEMENT, STATEMENT_END, to_ret, another_node);
    to_ret->parent = to_ret_2;
    if (another_node != NULL) another_node->parent = to_ret_2;

    return to_ret_2;
//     node_t* right = NULL;
    // if ((right = get_statement(tree, input, pos)) != NULL) ;

    // node_t* really_to_ret = new_node(tree, STATEMENT, STATEMENT_END, to_ret, right);
    // to_ret->parent = really_to_ret;

    // return NULL;
}

node_t* get_if_state(my_tree_t* tree, tokens* input, size_t* pos)
{
    if (CURR_TYPE != STATEMENT || (int) CURR_VAL != IF_STATE) return NULL;
    INCR;

    node_t* condition = get_expression(tree, input, pos);
    if (condition == NULL) CUSTOM_SYNTAX_ERROR("Expected expression in if at line %zu column %zu",
                                                   input[*pos].line, input[*pos].column);

    if ((int) CURR_VAL != CONDITION_END) SYNTAX_ERROR(all_ops[CONDITION_END].text);
    INCR;

    node_t* doing = get_statement(tree, input, pos);
    if (doing == NULL) CUSTOM_SYNTAX_ERROR("Expected some statements after if at line %zu column %zu",
                                                   input[*pos].line, input[*pos].column);

    node_t* to_ret = new_node(tree, STATEMENT, IF_STATE, condition, doing);
    doing->parent = condition->parent = to_ret;

    return to_ret;
}

