#include "make_assembler_file.h"

err_code_t generate_assembler(my_tree_t* tree, const char* filename)
{
    assert(tree);
    assert(filename);

    FILE* SAFE_OPEN_FILE(output, filename, "w");

    write_to_assembler(output, tree, tree->root);

    fclose(output);

    return OK;
}

#define PRINT(...) fprintf(output, __VA_ARGS__);

err_code_t write_to_assembler(FILE* output, my_tree_t* tree, node_t* curr_node)
{
    if (curr_node->type != STATEMENT && curr_node->type != OP)
    {
        fprintf(stderr, "Type is not statement");
        return UNKNOWN;
    }

    switch ((int) curr_node->data)
    {
        case STATEMENT_END:
        {
            write_to_assembler(output, tree, curr_node->left);
            break;
        }
        case EQUAL:
        {
            write_equal(output, tree, curr_node);
            break;
        }
    }

    if (curr_node->right != NULL) write_to_assembler(output, tree, curr_node->right);
    else                          PRINT("hlt\n")

    return OK;
}

err_code_t write_expression(FILE* output, my_tree_t* tree, node_t* node)
{
    switch (node->type)
    {
        case NUM:
        {
            PRINT("push %lg\n", node->data);
            return OK;
        }
        case OP :
        {
            write_expression(output, tree, node->right);
            write_expression(output, tree, node->left);
            PRINT("%s\n",       all_ops[(int) node->data].assembler_text);
            return OK;
        }
        default:
        {
            fprintf(stderr, "Unknown operation\n");
        }
    }

    return OK;
}

err_code_t write_equal(FILE* output, my_tree_t* tree, node_t* node)
{
    write_expression(output, tree, node->right);
    PRINT("pop [0]\n");

    return OK;
}

err_code_t write_print(FILE* output, my_tree_t* tree, node_t* node)
{
    write_expression(output, tree, node->left);
    PRINT("out");

    return OK;
}
