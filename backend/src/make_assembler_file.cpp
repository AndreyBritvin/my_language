#include "make_assembler_file.h"

err_code_t generate_assembler(my_tree_t* tree, const char* filename)
{
    assert(tree);
    assert(filename);

    FILE* SAFE_OPEN_FILE(output, filename, "w");

    write_to_assembler(output, tree, tree->root, 0);

    fclose(output);

    return OK;
}

#define PRINT(...) fprintf(output, __VA_ARGS__);
#define PRINT_KW(word) PRINT("%s\n", all_ops[word].assembler_text);
#define PRINT_KW_WO_NL(word) PRINT("%s ", all_ops[word].assembler_text);

err_code_t write_to_assembler(FILE* output, my_tree_t* tree, node_t* curr_node, size_t recurs_level)
{
    if (curr_node->type != STATEMENT && curr_node->type != OP)
    {
        fprintf(stderr, "Type is not statement\n");
        return UNKNOWN;
    }

    switch ((int) curr_node->data)
    {
        case STATEMENT_END:
        {
            write_to_assembler(output, tree, curr_node->left, recurs_level);
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
        case RETURN:
        {
            write_return(output, tree, curr_node);
            break;
        }
        case FUNC_CALL:
        {
            write_func_call(output, tree, curr_node);
            break;
        }case FUNC_DECL:
        {
            write_func_decl(output, tree, curr_node, recurs_level);
            break;
        }
    }
    // PRINT("\n");

    if (curr_node->right != NULL && curr_node->type == STATEMENT && (int) curr_node->data == STATEMENT_END)
                                         write_to_assembler(output, tree, curr_node->right, recurs_level);
    else if (recurs_level == 0 && curr_node->type == STATEMENT && (int) curr_node->data == STATEMENT_END)
                                         PRINT("hlt\n")

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
            write_expression(output, tree, node->left);
            write_expression(output, tree, node->right);
            PRINT("%s\n", all_ops[(int) node->data].assembler_text);
            return OK;
        }
        case VAR:
        {
            write_var(output, tree, node, VAR_PUSH);
            return OK;
        }
        case STATEMENT:
        {
            if ((int) node->data == FUNC_CALL)
            {
                write_func_call(output, tree, node);
                return OK;
            }
        }
        default:
        {
            fprintf(stderr, "Unknown operation\n");
        }
    }

    return OK;
}

err_code_t write_var(FILE* output, my_tree_t* tree, node_t* node, var_writing is_push)
{
    if (is_push == VAR_PUSH)
    {
        PRINT("push [0]\n");
    }
    else if (is_push == VAR_POP)
    {
        PRINT("pop [0]\n");
    }
    else if (is_push == VAR_NAME)
    {
        PRINT("%s", *(char**)&node->data);
    }

    return OK;
}

err_code_t write_equal(FILE* output, my_tree_t* tree, node_t* node)
{
    write_expression(output, tree, node->right);
    write_var(output, tree, node->left, VAR_POP);
    PRINT("\n");

    return OK;
}

err_code_t write_print(FILE* output, my_tree_t* tree, node_t* node)
{
    write_expression(output, tree, node->left);
    PRINT_KW(PRINT_STATE)
    PRINT("\n");

    return OK;
}

err_code_t write_if(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level)
{
    static int if_label_counter = 0;

    write_var(output, tree, node->left->left, VAR_PUSH); // left part should be before
    write_expression(output, tree, node->left->right);

    write_if_operator(output, tree, node->left);

    PRINT(" IF_LABEL_%d:\n", if_label_counter);

    write_to_assembler(output, tree, node->right, recurs_level + 1);

    PRINT("IF_LABEL_%d:\n\n", if_label_counter);

    if_label_counter++;

    return OK;
}

err_code_t write_if_operator(FILE* output, my_tree_t* tree, node_t* node)
{
    switch ((int) node->data)
    {
        case MORE:       PRINT("jbe"); break;
        case LESS:       PRINT("jae"); break;
        case MORE_EQUAL: PRINT("jb"); break;
        case LESS_EQUAL: PRINT("ja"); break;
        case NOOO_EQUAL: PRINT("je"); break;
        case FULL_EQUAL: PRINT("jne"); break;
        default: fprintf(stderr, "Unknown comp operator %lg\n", node->data);
    }

    return OK;
}

err_code_t write_while(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level)
{
    static int while_label_counter = 0;

    PRINT("WHILE_LABEL_%d:\n", while_label_counter); // TODO: add index

    write_var       (output, tree, node->left->left, VAR_PUSH); // left part should be before
    write_expression(output, tree, node->left->right);

    write_if_operator(output, tree, node->left);

    PRINT(" END_WHILE_%d:\n", while_label_counter);

    write_to_assembler(output, tree, node->right, recurs_level + 1);

    PRINT("jump WHILE_LABEL_%d:\n", while_label_counter);
    PRINT("END_WHILE_%d:\n\n", while_label_counter);

    return OK;
}

err_code_t write_return(FILE* output, my_tree_t* tree, node_t* node)
{
    write_expression(output, tree, node->left);
    PRINT("push ax\n"); // return value
    PRINT_KW(RETURN);
    PRINT("\n");

    return OK;
}

err_code_t write_func_call(FILE* output, my_tree_t* tree, node_t* node)
{
    PRINT_KW_WO_NL(FUNC_CALL); // TODO: add vars
    write_var(output, tree, node->left->left, VAR_NAME);
    PRINT(":\n");
    PRINT("pop ax\n");

    return OK;
}

err_code_t write_func_decl(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level)
{
    PRINT("jump ");                                     // jump func_end
    write_var(output, tree, node->left->left, VAR_NAME);
    PRINT("_END:\n");

    write_var(output, tree, node->left->left, VAR_NAME);
    PRINT(":\n");
    // TODO: write parametrs
    write_to_assembler(output, tree, node->right, recurs_level + 1);

    write_var(output, tree, node->left->left, VAR_NAME); // func_END:
    PRINT("_END:\n\n");

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
