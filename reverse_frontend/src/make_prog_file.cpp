#include "make_prog_file.h"

err_code_t generate_prog_file(my_tree_t* tree, const char* filename)
{
    assert(tree);
    assert(filename);

    FILE* SAFE_OPEN_FILE(output, filename, "w");

    write_to_file(output, tree, tree->root, 0);

    fclose(output);

    return OK;
}

#define PRINT(...) fprintf(output, __VA_ARGS__);
#define KEYWORD(name) all_ops[name].text
#define PRINT_KW(name)          PRINT("%s ", KEYWORD(name));
#define PRINT_KW_WO_SPACE(name) PRINT("%s",  KEYWORD(name));

err_code_t write_to_file(FILE* output, my_tree_t* tree, node_t* curr_node, size_t recurs_level)
{
    if (curr_node->type != STATEMENT && curr_node->type != OP)
    {
        fprintf(stderr, "Type is not statement\n");
        return UNKNOWN;
    }

    if ((int) curr_node->data == STATEMENT_END)
    {
        print_tabs(output, recurs_level);
    }
    switch ((int) curr_node->data)
    {
        case STATEMENT_END:
        {
            write_to_file(output, tree, curr_node->left, recurs_level);
            break;
        }
        case EQUAL_BEGIN:
        {
            write_equal(output, tree, curr_node);
            break;
        }
        case PRINT_STATE:
        {
            write_print(output, tree, curr_node);
            break;
        }
        case IF_STATE:
        {
            write_if(output, tree, curr_node, recurs_level);
            break;
        }
        case WHILE_STATE:
        {
            write_while(output, tree, curr_node, recurs_level);
            break;
        }
        case FUNC_DECL:
        {
            write_func_decl(output, tree, curr_node, recurs_level);
            break;
        }
        case FUNC_CALL:
        {
            write_func_call(output, tree, curr_node);
            PRINT_KW(STATEMENT_END);
            PRINT("\n");
            break;
        }
        case RETURN:
        {
            write_return(output, tree, curr_node);
            break;
        }
    }

    if (curr_node->right != NULL && curr_node->type == STATEMENT && (int) curr_node->data == STATEMENT_END)
    write_to_file(output, tree, curr_node->right, recurs_level);
    // else                          PRINT("hlt\n")

    return OK;
}

err_code_t write_expression(FILE* output, my_tree_t* tree, node_t* node) // TODO: remove extra spaces
{
    switch (node->type)
    {
        case NUM:
        {
            PRINT("%lg ", node->data);
            return OK;
        }
        case OP :
        {
            write_expression(output, tree, node->left);
            PRINT_KW((int) node->data);
            write_expression(output, tree, node->right);
            return OK;
        }
        case VAR:
        {
            write_var(output, tree, node, true);
            return OK;
        }
        case STATEMENT:
        {
            write_func_call(output, tree, node);
            return OK;
        }
        default:
        {
            fprintf(stderr, "Unknown operation\n");
        }
    }

    return OK;
}

err_code_t write_var(FILE* output, my_tree_t* tree, node_t* node, bool is_space)
{
    if (is_space) {PRINT("%s ", *(char**)&node->data);}
    else          {PRINT("%s",  *(char**)&node->data);}

    return OK;
}

err_code_t write_equal(FILE* output, my_tree_t* tree, node_t* node)
{
    PRINT_KW(EQUAL_BEGIN);
    write_var(output, tree, node->left, true);
    PRINT_KW(EQUAL_MIDDLE);
    write_expression(output, tree, node->right);
    PRINT_KW(STATEMENT_END);
    PRINT("\n");

    return OK;
}

err_code_t write_print(FILE* output, my_tree_t* tree, node_t* node)
{
    PRINT_KW(PRINT_STATE);
    write_expression(output, tree, node->left);
    PRINT_KW(STATEMENT_END);
    PRINT("\n");

    return OK;
}

err_code_t write_if(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level)
{
    PRINT_KW(IF_STATE);
    write_expression(output, tree, node->left);
    PRINT_KW(CONDITION_END); PRINT("\n");
    print_tabs(output, recurs_level);
    PRINT_KW(SCOPE_OPEN);    PRINT("\n");
    write_to_file(output, tree, node->right, recurs_level + 1);
    print_tabs(output, recurs_level);
    PRINT_KW(SCOPE_CLOS);
    PRINT("\n");

    return OK;
}

err_code_t write_while(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level)
{
    PRINT_KW(WHILE_STATE);
    write_expression(output, tree, node->left);
    PRINT_KW(CONDITION_END); PRINT("\n");
    print_tabs(output, recurs_level);
    PRINT_KW(SCOPE_OPEN);    PRINT("\n");
    write_to_file(output, tree, node->right, recurs_level + 1);
    print_tabs(output, recurs_level);
    PRINT_KW(SCOPE_CLOS);
    PRINT("\n");

    return OK;
}

err_code_t print_tabs(FILE* output, size_t recurs_level)
{
    //PRINT("%zu", recurs_level);
    for (size_t i = 0; i < recurs_level; i++)
    {
        PRINT("    ");
    }

    return OK;
}

err_code_t write_func_decl(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level)
{
    PRINT_KW(FUNC_DECL);
    write_var(output, tree, node->left->left, false);

    PRINT_KW_WO_SPACE(BRACKET_OPEN);
    write_args(output, tree, node->left->right);
    PRINT_KW_WO_SPACE(BRACKET_CLOS);  PRINT("\n");

    print_tabs(output, recurs_level);
    PRINT_KW(SCOPE_OPEN);    PRINT("\n");
    write_to_file(output, tree, node->right, recurs_level + 1);
    print_tabs(output, recurs_level);
    PRINT_KW(SCOPE_CLOS);

    PRINT("\n");

    return OK;
}

err_code_t write_func_call(FILE* output, my_tree_t* tree, node_t* node)
{
    write_var(output, tree, node->left->left, false);

    PRINT_KW_WO_SPACE(BRACKET_OPEN);
    write_args(output, tree, node->left->right);
    PRINT_KW(BRACKET_CLOS);

    return OK;
}

err_code_t write_return(FILE* output, my_tree_t* tree, node_t* node)
{
    PRINT_KW(RETURN);
    write_expression(output, tree, node->left);
    PRINT_KW(STATEMENT_END);
    PRINT("\n");

    return OK;
}

err_code_t write_args(FILE* output, my_tree_t* tree, node_t* node)
{
    if (node->left  != NULL)
    {
        write_expression(output, tree, node->left);
        fseek(output, -1, SEEK_END); // delete extra space
        if (node->right != NULL) {PRINT_KW(SEPARATOR);}
    }
    if (node->right != NULL) write_args      (output, tree, node->right);

    return OK;
}
