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
#define SYNTAX_ERROR(expected) {fprintf(stderr, "At line %zu column %zu expected %s, but %c (%d) instead\n",\
                                            input[*pos].line, input[*pos].column, expected, CURR_VAL, CURR_VAL);\
                                             abort();}

#define REQUIRE(symbol) {if (CURR_TYPE != OP && CURR_TYPE != STATEMENT || (int) CURR_VAL != symbol)  \
                        SYNTAX_ERROR(all_ops[symbol].text);                                         \
                        INCR;}

#define CHECK_VALUE(symbol)                                                                       \
                        if (CURR_TYPE != STATEMENT || (int) CURR_VAL != symbol) return NULL; \
                        INCR;

#define REQUIRE_EXPR(name)                                                                          \
                        node_t* name = get_expression(tree, input, pos);                            \
                        if (name == NULL)                                                           \
                        CUSTOM_SYNTAX_ERROR("Expected expression at line %zu column %zu",           \
                                                   input[*pos].line, input[*pos].column);

#define REQUIRE_ID(name)                                                                           \
                        node_t* name = get_variable(tree, input, pos);                              \
                        if (name == NULL)                                                           \
                        CUSTOM_SYNTAX_ERROR("Expected variable at line %zu column %zu",             \
                                                   input[*pos].line, input[*pos].column);

#define REQUIRE_STATEMENT(name)                                                                     \
                        node_t* name = get_statement(tree, input, pos);                             \
                        if (name == NULL)                                                           \
                        CUSTOM_SYNTAX_ERROR("Expected statement at line %zu column %zu",            \
                                                   input[*pos].line, input[*pos].column);

#define REQUIRE_RETURN(name)                                                                     \
                        if (check_return_to_the_end(tree, func_body) == NULL)                    \
                        CUSTOM_SYNTAX_ERROR("Expected return at line %zu column %zu",            \
                                                   input[*pos].line, input[*pos].column);

#define REQUIRE_CUSTOM(name, func)                                                                  \
                        node_t* name = func(tree, input, pos);                                      \
                        if (name == NULL)                                                           \
                        CUSTOM_SYNTAX_ERROR("Expected comparison at line %zu column %zu",           \
                                                   input[*pos].line, input[*pos].column);

#define SEPARATOR_NODE new_node(tree, STATEMENT, SEPARATOR, NULL, NULL)

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
    INCR;

    REQUIRE(BRACKET_OPEN);

    node_t* left_subtree = get_expression(tree, input, pos);

    REQUIRE(BRACKET_CLOS);

    node_t* to_ret = new_node(tree, OP, func, left_subtree, NULL);
    left_subtree->parent = to_ret;

    return to_ret;
}

node_t* get_exp(my_tree_t* tree, tokens* input, size_t* pos)
{
    node_t* val = get_primary(tree, input, pos);
    while ((int) CURR_VAL == EXP)
    {
        INCR;
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
        INCR;
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
        INCR;
        node_t* val = get_expression(tree, input, pos);
        if (CURR_TYPE != OP || (int) CURR_VAL != BRACKET_CLOS) SYNTAX_ERROR(all_ops[BRACKET_CLOS].text);
        INCR;
        return val;
    }
    node_t* to_ret = NULL;
    if ((to_ret = get_func_call(tree, input, pos)) != NULL) return to_ret;
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
        INCR;
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

    INCR;

    return new_node(tree, NUM, val, NULL, NULL);
}

node_t* get_variable(my_tree_t* tree, tokens* input, size_t* pos)
{
    if (CURR_TYPE != VAR) return NULL;

    node_t* to_ret = NULL;

    printf("Well, we are in get_variable. Type = %d, addr in value is %p and str is \n",
             CURR_TYPE, *(char**)&CURR_VAL);

    to_ret = new_node(tree, VAR, '\0', NULL, NULL);
    memcpy(&to_ret->data, &CURR_VAL, sizeof(tree_val_t));
    INCR;

    return to_ret;
}

node_t* get_assingnment(my_tree_t* tree, tokens* input, size_t* pos)
{
    CHECK_VALUE(EQUAL_BEGIN);

    REQUIRE_ID(identificator);

    REQUIRE(EQUAL_MIDDLE);

    REQUIRE_EXPR(right_expr);

    node_t* equal_node = new_node(tree, STATEMENT, EQUAL_BEGIN, identificator, right_expr);
    identificator->parent = right_expr->parent = equal_node;

    REQUIRE(STATEMENT_END);

    return equal_node;
}

node_t* get_statement(my_tree_t* tree, tokens* input, size_t* pos)
{
    if (CURR_TYPE == STATEMENT && (int) CURR_VAL == SCOPE_OPEN)
    {
        INCR;
        node_t* inside = get_statement(tree, input, pos);
        REQUIRE(SCOPE_CLOS);

        return inside;
    }
    node_t* state = NULL;
    if      ((state = get_func_call  (tree, input, pos)) != NULL)  REQUIRE(STATEMENT_END)
    else if ((state = get_if_state   (tree, input, pos)) != NULL)  state;
    else if ((state = get_while_state(tree, input, pos)) != NULL)  state;
    else if ((state = get_assingnment(tree, input, pos)) != NULL)  state;
    else if ((state = get_print_state(tree, input, pos)) != NULL)  state;
    else if ((state = get_return     (tree, input, pos)) != NULL)  state;
    else if ((state = get_func_decl  (tree, input, pos)) != NULL)  state;

    if (state == NULL) return NULL;

    node_t* another_node = get_statement(tree, input, pos);

    node_t* to_ret_2 = new_node(tree, STATEMENT, STATEMENT_END, state, another_node);
    state->parent = to_ret_2;
    if (another_node != NULL) another_node->parent = to_ret_2;

    return to_ret_2;
}

node_t* get_if_state(my_tree_t* tree, tokens* input, size_t* pos)
{
    CHECK_VALUE(IF_STATE);

    REQUIRE_ID(cond_var)
    REQUIRE_CUSTOM(condition, get_comparison);
    REQUIRE_EXPR(cond_expr);

    condition->left  = cond_var;
    condition->right = cond_expr;
    cond_var->parent = cond_expr->parent = condition;

    REQUIRE(CONDITION_END);

    REQUIRE_STATEMENT(doing);

    node_t* to_ret = new_node(tree, STATEMENT, IF_STATE, condition, doing);
    doing->parent = condition->parent = to_ret;

    return to_ret;
}

node_t* get_while_state(my_tree_t* tree, tokens* input, size_t* pos)
{
    CHECK_VALUE(WHILE_STATE);

    REQUIRE_ID(cond_var)
    REQUIRE_CUSTOM(condition, get_comparison);
    REQUIRE_EXPR(cond_expr);

    condition->left  = cond_var;
    condition->right = cond_expr;
    cond_var->parent = cond_expr->parent = condition;

    REQUIRE(CONDITION_END);

    REQUIRE_STATEMENT(doing);

    node_t* to_ret = new_node(tree, STATEMENT, WHILE_STATE, condition, doing);
    doing->parent = condition->parent = to_ret;

    return to_ret;
}

node_t* get_print_state(my_tree_t* tree, tokens* input, size_t* pos)
{
    CHECK_VALUE(PRINT_STATE);

    REQUIRE_EXPR(printable);

    REQUIRE(STATEMENT_END);

    node_t* to_ret = new_node(tree, STATEMENT, PRINT_STATE, printable, NULL);
    printable->parent = to_ret;

    return to_ret;
}

node_t* get_comparison(my_tree_t* tree, tokens* input, size_t* pos)
{
    node_t* comparison_node = new_node(tree, OP, (int) CURR_VAL, NULL, NULL);

    if (CURR_TYPE != OP || (int) CURR_VAL > FULL_EQUAL || (int) CURR_VAL < MORE)
    SYNTAX_ERROR(all_ops[NOOO_EQUAL].text);
    INCR;

    return comparison_node;
}

node_t* get_return(my_tree_t* tree, tokens* input, size_t* pos)
{
    CHECK_VALUE(RETURN);

    REQUIRE_EXPR(return_expr);

    REQUIRE(STATEMENT_END);

    node_t* to_ret = new_node(tree, STATEMENT, RETURN, return_expr, NULL);
    return_expr->parent = to_ret;

    return to_ret;
}

#define REQUIRE_ID_SEQUENCE(type)                                                       \
    while (CURR_TYPE == VAR)                                                        \
    {                                                                               \
        REQUIRE##type(var_n);                                                         \
        separator_node->left = var_n;                                               \
        var_n->parent = separator_node;                                             \
                                                                                    \
        if (CURR_TYPE == OP && (int) CURR_VAL == BRACKET_CLOS) break;               \
        REQUIRE(SEPARATOR);                                                         \
        separator_node->right = SEPARATOR_NODE;   \
        separator_node->right->parent = separator_node;                             \
        separator_node = separator_node->right;                                     \
    }

node_t* get_func_decl(my_tree_t* tree, tokens* input, size_t* pos)
{
    CHECK_VALUE(FUNC_DECL);

    node_t* func_def = new_node(tree, STATEMENT, FUNC_DECL, NULL, NULL);

    REQUIRE_ID(func_name);

    REQUIRE(BRACKET_OPEN);

    node_t* separator_node = SEPARATOR_NODE;

    node_t* func_spec = new_node(tree, STATEMENT, FUNC_SPEC, func_name, separator_node);
    separator_node->parent = func_spec;
    func_name->parent = func_spec;
    func_spec->parent = func_def;
    func_def->left = func_spec;

    REQUIRE_ID_SEQUENCE(_ID);

    REQUIRE(BRACKET_CLOS);

    REQUIRE(SCOPE_OPEN);
    REQUIRE_STATEMENT(func_body);

    func_def->right   = func_body;
    func_body->parent = func_def;

    REQUIRE_RETURN();
    REQUIRE(SCOPE_CLOS);

    return func_def;
}

bool check_return_to_the_end(my_tree_t* tree, node_t* where_to_connect)
{
    if (where_to_connect->right == NULL)
    {
        if (where_to_connect->left->type != STATEMENT || (int) where_to_connect->left->data != RETURN)
        return false;
        return true;
        // node_t* separator_node  = new_node(tree, STATEMENT, STATEMENT_END, what_to_connect, NULL);
        // where_to_connect->right = separator_node;
        // separator_node->parent  = where_to_connect;
        // what_to_connect->parent = separator_node;
    }
    else
    {
        return check_return_to_the_end(tree, where_to_connect->right);
    }

    return false;
}

node_t* get_func_call(my_tree_t* tree, tokens* input, size_t* pos)
{
    if (CURR_TYPE == VAR && input[*pos + 1].type == OP && (int) input[*pos + 1].value == BRACKET_OPEN)
    {
        REQUIRE_ID(func_name);

        node_t* func_call = new_node(tree, STATEMENT, FUNC_CALL, NULL, NULL);

        REQUIRE(BRACKET_OPEN);

        node_t* separator_node = SEPARATOR_NODE;
        node_t* func_spec = new_node(tree, STATEMENT, FUNC_SPEC, func_name, separator_node);
        separator_node->parent = func_spec;
        func_name->parent      = func_spec;
        func_spec->parent      = func_call;
        func_call->left        = func_spec;

        while (CURR_TYPE != OP || (int) CURR_VAL != BRACKET_CLOS)                       \
        {                                                                               \
            REQUIRE_EXPR(var_n);                                                         \
            separator_node->left = var_n;                                               \
            var_n->parent = separator_node;                                             \
                                                                                        \
            if (CURR_TYPE == OP && (int) CURR_VAL == BRACKET_CLOS) break;               \
            REQUIRE(SEPARATOR);                                                         \
            separator_node->right = SEPARATOR_NODE;   \
            separator_node->right->parent = separator_node;                             \
            separator_node = separator_node->right;                                     \
        }

        REQUIRE(BRACKET_CLOS);

        return func_call;
    }

    return NULL;
}
