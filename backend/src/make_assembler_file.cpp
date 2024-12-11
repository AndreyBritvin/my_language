#include "make_assembler_file.h"

err_code_t generate_assembler(my_tree_t* tree, const char* filename)
{
    assert(tree);
    assert(filename);

    FILE* SAFE_OPEN_FILE(output, filename, "w");

    write_equal(output, tree, tree->root->left);

    fclose(output);

    return OK;
}

#define PRINT(...) fprintf(output, __VA_ARGS__);

err_code_t write_to_assembler(FILE* output, my_tree_t* tree, node_t* curr_node)
{
    switch (curr_node->type)
    {
        case NUM: PRINT("push %lg\n", curr_node->data); return OK;
        case OP : PRINT("%s\n",       all_ops[(int) curr_node->data].assembler_text); return OK;
    }

    return OK;
}

err_code_t write_math_operation(FILE* output, my_tree_t* tree, node_t* node)
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
            write_math_operation(output, tree, node->right);
            write_math_operation(output, tree, node->left);
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
    write_math_operation(output, tree, node->right);
    PRINT("pop [0]");

    return OK;
}
