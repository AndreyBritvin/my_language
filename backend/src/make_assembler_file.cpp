#include "make_assembler_file.h"
#include "name_table.h"

#define PRINT(...) fprintf(output, __VA_ARGS__);
#define PRINT_KW(word) PRINT("%s\n", all_ops[word].assembler_text);
#define PRINT_KW_WO_NL(word) PRINT("%s ", all_ops[word].assembler_text);

#define FUNC_NAME_BY_NODE(node) *(char**)&node->left->left->data

err_code_t generate_assembler(my_tree_t* tree, const char* filename, nametable_t nt)
{
    assert(tree);
    assert(filename);
    // nametable = nt;
    FILE* SAFE_OPEN_FILE(output, filename, "w");
    PRINT("push %zu\n"
          "pop bx  \n", get_num_of_global_vars(nt));

    write_to_assembler(output, tree, tree->root, 0, nt);

    fclose(output);

    return OK;
}


err_code_t write_to_assembler(FILE* output, my_tree_t* tree, node_t* curr_node, size_t recurs_level, nametable_t nametable)
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
            write_to_assembler(output, tree, curr_node->left, recurs_level, nametable);
            break;
        }
        case EQUAL_BEGIN:
        {
            write_equal(output, tree, curr_node, nametable);
            break;
        }
        case PRINT_STATE:
        {
            write_print(output, tree, curr_node, nametable);
            break;
        }
        case IF_STATE:
        {
            write_if(output, tree, curr_node, recurs_level, nametable);
            break;
        }
        case WHILE_STATE:
        {
            write_while(output, tree, curr_node, recurs_level, nametable);
            break;
        }
        case RETURN:
        {
            write_return(output, tree, curr_node, nametable);
            break;
        }
        case FUNC_CALL:
        {
            write_func_call(output, tree, curr_node, nametable);
            break;
        }case FUNC_DECL:
        {
            write_func_decl(output, tree, curr_node, recurs_level, nametable);
            break;
        }
    }
    // PRINT("\n");

    if (curr_node->right      != NULL
     && curr_node->type       == STATEMENT
     && (int) curr_node->data == STATEMENT_END
       )
    {
        write_to_assembler(output, tree, curr_node->right, recurs_level, nametable);
    }
    else if (recurs_level          == 0
          && curr_node->type       == STATEMENT
          && (int) curr_node->data == STATEMENT_END
            )
    {
         PRINT("hlt\n")
    }

    return OK;
}

err_code_t write_expression(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable)
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
            write_expression(output, tree, node->left, nametable);
            write_expression(output, tree, node->right, nametable);
            PRINT("%s\n", all_ops[(int) node->data].assembler_text);
            return OK;
        }
        case VAR:
        {
            write_var(output, tree, node, VAR_PUSH, nametable);
            return OK;
        }
        case STATEMENT:
        {
            if ((int) node->data == FUNC_CALL)
            {
                write_func_call(output, tree, node, nametable);
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

err_code_t write_var(FILE* output, my_tree_t* tree, node_t* node, var_writing is_push, nametable_t nametable)
{
    char* id_name = *(char**)&node->data;
    char* bx_offset = "";
    size_t elem_num = get_element_index(nametable, id_name);

    if (nametable[elem_num].dependence != MAX_ID_COUNT) bx_offset = "bx + ";

    size_t id_full_index = nametable[elem_num].full_index;
    if (is_push == VAR_PUSH)
    {
        PRINT("push [%s%d] ; \n", bx_offset, id_full_index);//, id_name);
    }
    else if (is_push == VAR_POP)
    {
        PRINT("pop [%s%d] ; \n", bx_offset, id_full_index);//, id_name);
    }
    else if (is_push == VAR_NAME)
    {
        PRINT("%s", id_name);
    }

    return OK;
}

err_code_t write_equal(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable)
{
    write_expression(output, tree, node->right, nametable);
    write_var(output, tree, node->left, VAR_POP, nametable);
    PRINT("\n");

    return OK;
}

err_code_t write_print(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable)
{
    write_expression(output, tree, node->left, nametable);
    PRINT_KW(PRINT_STATE)
    PRINT("\n");

    return OK;
}

err_code_t write_if(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level, nametable_t nametable)
{
    static int if_label_counter = 0;
    int buffer_index = if_label_counter;
    write_var(output, tree, node->left->left, VAR_PUSH, nametable); // left part should be before
    write_expression(output, tree, node->left->right, nametable);

    write_if_operator(output, tree, node->left, nametable);

    PRINT(" IF_LABEL_%d:\n", if_label_counter++);

    write_to_assembler(output, tree, node->right, recurs_level + 1, nametable);

    PRINT("IF_LABEL_%d:\n\n", buffer_index);

    if_label_counter++;

    return OK;
}

err_code_t write_if_operator(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable)
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

err_code_t write_while(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level, nametable_t nametable)
{
    static int while_label_counter = 0;
    int buffer_counter = while_label_counter;

    PRINT("WHILE_LABEL_%d:\n", while_label_counter);

    write_var       (output, tree, node->left->left, VAR_PUSH, nametable); // left part should be before
    write_expression(output, tree, node->left->right, nametable);

    write_if_operator(output, tree, node->left, nametable);

    PRINT(" END_WHILE_%d:\n", while_label_counter++);

    write_to_assembler(output, tree, node->right, recurs_level + 1, nametable);

    PRINT("jump WHILE_LABEL_%d:\n", buffer_counter);
    PRINT("END_WHILE_%d:\n\n", buffer_counter);

    return OK;
}

err_code_t write_return(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable)
{
    write_expression(output, tree, node->left, nametable);
    PRINT("pop ax\n"); // return value
    PRINT_KW(RETURN);
    PRINT("\n");

    return OK;
}

err_code_t write_func_call(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable)
{
    PRINT("push bx\n"); // copy

    write_parametrs(output, tree, node->left->right, 0, false, nametable);

    char* func_name = FUNC_NAME_BY_NODE(node);
    size_t func_id  = get_element_index(nametable, func_name);
    size_t func_above_id = get_current_func(node->parent, nametable);

    size_t local_vars = get_amount_of_local_vars_in_func(func_id, nametable);
    if (func_above_id != MAX_ID_COUNT)
    {
        PRINT("push bx + %zu\n", get_amount_of_local_vars_in_func(func_above_id, nametable));
        PRINT("pop  bx \n"); // bx += n of params
    }
    if (local_vars != 0) write_parametrs(output, tree, node->left->right, get_amount_of_parametrs(func_id, nametable) - 1, true, nametable);
    PRINT("dump\n");

    PRINT_KW_WO_NL(FUNC_CALL);
    write_var(output, tree, node->left->left, VAR_NAME, nametable);
    PRINT(":\n");

    PRINT("pop bx\n"); // return copy
    PRINT("push ax\n");

    return OK;
}

size_t get_current_func(node_t* node, nametable_t nametable)
{
    if (node->type == STATEMENT && (int) node->data == FUNC_DECL)
    {
        return get_element_index(nametable, FUNC_NAME_BY_NODE(node));
    }
    if (node->parent != NULL) return get_current_func(node->parent, nametable);

    return MAX_ID_COUNT;
}

err_code_t write_func_decl(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level, nametable_t nametable)
{
    PRINT("jump ");                                     // jump func_END:
    write_var(output, tree, node->left->left, VAR_NAME, nametable);
    PRINT("_END:\n");

    write_var(output, tree, node->left->left, VAR_NAME, nametable);
    PRINT(":\n");

    write_to_assembler(output, tree, node->right, recurs_level + 1, nametable);

    write_var(output, tree, node->left->left, VAR_NAME, nametable); // func_END:
    PRINT("_END:\n\n");

    return OK;
}

size_t get_amount_of_local_vars_in_func(size_t func_num, nametable_t nametable)
{
    assert(nametable[func_num].type == FUNC_TYPE);

    for (size_t i = func_num + 1; i < MAX_ID_COUNT; i++)
    {
        if (nametable[i].dependence != func_num
         || nametable[i].type       == FUNC_TYPE
         || nametable[i].name[0]    == '\0')
        {
            return i - func_num - 1; // amount means 1,2,3,4...
        }
    }

    return 0;
}
// TODO: think about merging up and down function
size_t get_amount_of_parametrs(size_t func_num, nametable_t nametable)
{
    assert(nametable[func_num].type == FUNC_TYPE);

    for (size_t i = func_num + 1; i < MAX_ID_COUNT; i++)
    {
        if (nametable[i].dependence != func_num
         || nametable[i].type       != PARAM_TYPE
         || nametable[i].name[0]    == '\0')
        {
            return i - func_num - 1; // amount means 1,2,3,4...
        }
    }
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

size_t get_num_of_global_vars(nametable_t nametable)
{
    size_t max_index = 0;
    for (size_t i = 0; i < MAX_ID_COUNT; i++)
    {
        if (nametable[i].full_index > max_index && nametable[i].dependence == MAX_ID_COUNT)
        {
            max_index = nametable[i].full_index;
        }
    }

    return max_index > 0 ? max_index + 1 : 0;
}

err_code_t write_parametrs(FILE* output, my_tree_t* tree, node_t* node, int recurs_level, bool is_memory, nametable_t nametable)
{
    if (node->left  != NULL && is_memory == false) write_expression(output, tree, node->left, nametable);
    if (node->left  != NULL && is_memory == true ) PRINT("pop [bx + %d]\n", recurs_level);
    if (node->right != NULL) write_parametrs(output, tree, node->right, recurs_level - 1, is_memory, nametable);

    return OK;
}
