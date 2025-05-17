#include "make_assembler_file.h"
#include "name_table.h"
#include "make_prog_file.h"

#define PRINT(...) fprintf(output, __VA_ARGS__);
#define PRINT_KW(word) PRINT("%s\n", all_ops[word].assembler_text);
#define PRINT_KW_WO_NL(word) PRINT("%s ", all_ops[word].assembler_text);

#define FUNC_NAME_BY_NODE(node) *(char**)&node->left->left->data
#define COMMENT(node, comment) PRINT(";"); PRINT("%s", comment);\
                               write_expression(output, tree, node); PRINT("\n");
#define PASTE_SPACES_IN_COMMENT bx_offset[0] == '\0' ? "     " : ""

#define PRINT_ARG node->left
#define COMP_LEFT_ARG  node->left->left
#define COMP_RIGHT_ARG node->left->right
#define SCOPE_EXPRESSION node->right
#define FUNC_NAME_NODE node->left->left
#define FUNC_PARM_NODE node->left->right

err_code_t generate_assembler(my_tree_t* tree, const char* filename, nametable_t nt)
{
    assert(tree);
    assert(filename);
    // nametable = nt;

    FILE* SAFE_OPEN_FILE(output, filename, "w");
    PRINT("BITS 64\n" // for pop and push in 64bit mode
          );

    PRINT("section .text\n"
          "global main\n" // todo: add external printf/input?
          "extern out\n"
          "extern hlt\n"
          "%%macro XMM_POP 1\n"
          "movsd %1, [rsp]\n"
          "add rsp, 8\n"
          "%%endmacro\n"
          "%%macro XMM_PUSH 1\n"
          "sub rsp, 8\n"
          "movsd [rsp], %1\n"
          "%%endmacro\n"
          "\n\n"
          "main:\n"
          "push rbp\n"
          "mov rbp, rsp\n"
          "mov rbx, rbp\n" // save this for "main" global vars
          "sub rsp, %zu\n", (get_num_of_global_vars(nt)) * 8);

    write_to_assembler(output, tree, tree->root, 0, nt);

    PRINT("mov rsp, rbp\n"
          "pop rbp\n"
          "ret\n")

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
         PRINT("call hlt\n")
    }

    return OK;
}

err_code_t write_expression(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable)
{
    switch (node->type)
    {
        case NUM:
        {
            PRINT("mov rax, __float64__(%lf)\n", node->data);
            PRINT("movq xmm1, rax\n")
            PRINT("XMM_PUSH xmm1\n");
            return OK;
        }
        case OP :
        {
            write_expression(output, tree, node->left, nametable);
            write_expression(output, tree, node->right, nametable);
            PRINT("XMM_POP xmm2\n"
                  "XMM_POP xmm1\n");
            if ((int) node->data == EXP)
            {
                PRINT("%s xmm1, xmm1\n", all_ops[(int) node->data].assembler_text);
            }
            else
            {
                PRINT("%s xmm1, xmm2\n", all_ops[(int) node->data].assembler_text);
            }
            PRINT("XMM_PUSH xmm1\n");

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
    char* bx_offset = "rbx - ";

    size_t elem_num = get_element_index(nametable, id_name);
    size_t id_full_index = (nametable[elem_num].full_index + 1) * 8;

    if (nametable[elem_num].dependence != MAX_ID_COUNT)  bx_offset = "rbp - 8 -";
    if (nametable[elem_num].type       == PARAM_TYPE  ) {bx_offset = "rbp + 8 + ";
    }
    if(get_current_func(node, nametable) != MAX_ID_COUNT)
    id_full_index = get_amount_of_parametrs(get_element_index(nametable, nametable[get_current_func(node, nametable)].name), nametable) * 8 - id_full_index + 8;

    if(get_current_func(node, nametable) == MAX_ID_COUNT)
    id_full_index = get_num_of_global_vars(nametable) * 8 - id_full_index + 8;

    if (is_push == VAR_PUSH)
    {
        PRINT("movsd xmm1, [%s%d]             %s; %s\n", bx_offset, id_full_index,
                        PASTE_SPACES_IN_COMMENT, id_name);
        PRINT("XMM_PUSH xmm1\n");
    }
    else if (is_push == VAR_POP)
    {
        PRINT("XMM_POP xmm1\n")
        PRINT("movsd [%s%d], xmm1             %s; %s\n", bx_offset, id_full_index,
                        PASTE_SPACES_IN_COMMENT, id_name);
    }
    else if (is_push == VAR_NAME)
    {
        PRINT("%s", id_name);
    }

    return OK;
}

err_code_t write_equal(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable)
{
    write_expression(output, tree, SCOPE_EXPRESSION, nametable);
    write_var(output, tree, node->left, VAR_POP, nametable);
    COMMENT(node->right, "put in this ^ value: ");
    PRINT("\n");

    return OK;
}

err_code_t align_rsp(FILE* output)
{
    static int number_of_aligned = 0;
    PRINT("mov rax, rsp\n"
          "and rax, 0xF\n"
          "cmp rax, 8\n"
          "jne .already_aligned_%d\n"
          "sub rsp, 8\n"
          ".already_aligned_%d:\n", number_of_aligned, number_of_aligned);

    number_of_aligned++;

    return OK;
}

err_code_t write_print(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable)
{
    write_expression(output, tree, PRINT_ARG, nametable);
    PRINT("XMM_POP xmm0\n");
    align_rsp(output);
    PRINT_KW(PRINT_STATE)
    COMMENT(PRINT_ARG, "print ");
    PRINT("\n");

    return OK;
}

err_code_t write_if(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level, nametable_t nametable)
{
    static int if_label_counter = 0;
    int buffer_index = if_label_counter;
    write_var(output, tree, COMP_LEFT_ARG, VAR_PUSH, nametable); // left part should be before
    write_expression(output, tree, COMP_RIGHT_ARG, nametable);

    write_if_operator(output, tree, node->left, nametable);

    PRINT(" IF_LABEL_%d\n", if_label_counter++);

    write_to_assembler(output, tree, SCOPE_EXPRESSION, recurs_level + 1, nametable);

    PRINT("IF_LABEL_%d:\n\n", buffer_index);

    if_label_counter++;

    return OK;
}

err_code_t write_if_operator(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable)
{
    PRINT("XMM_POP xmm1\n"
          "XMM_POP xmm2\n"
          "comisd xmm2, xmm1\n")
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

    write_var       (output, tree, COMP_LEFT_ARG , VAR_PUSH, nametable); // left part should be before
    write_expression(output, tree, COMP_RIGHT_ARG,           nametable);

    write_if_operator(output, tree, node->left, nametable);

    PRINT(" END_WHILE_%d\n", while_label_counter++);

    write_to_assembler(output, tree, SCOPE_EXPRESSION, recurs_level + 1, nametable);

    PRINT("jmp WHILE_LABEL_%d\n", buffer_counter);
    PRINT("END_WHILE_%d:\n\n", buffer_counter);

    return OK;
}

err_code_t write_return(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable)
{
    write_expression(output, tree, node->left, nametable);
    PRINT("XMM_POP xmm0 ; save return value to xmm0\n"); // return value
    PRINT("leave\n");
    PRINT_KW(RETURN);
    PRINT("\n");

    return OK;
}

err_code_t write_func_call(FILE* output, my_tree_t* tree, node_t* node, nametable_t nametable)
{

    // PRINT("dump\n");
    char* func_name = FUNC_NAME_BY_NODE(node);
    size_t func_id  = get_element_index(nametable, func_name);
    size_t func_above_id = get_current_func(node->parent, nametable);

    size_t local_vars = get_amount_of_local_vars_in_func(func_id, nametable);
    write_parametrs(output, tree, FUNC_PARM_NODE, 0, false, nametable);

    if (func_above_id != MAX_ID_COUNT)
    {
        // PRINT("add rbp, %zu\n", get_amount_of_local_vars_in_func(func_above_id, nametable) * 8);
        // PRINT("pop  rbp \n"); // rbp += n of params TODO: rewrite with "add"
    }
    if (local_vars != 0)
    {
        write_parametrs(output, tree, FUNC_PARM_NODE,
                        get_amount_of_parametrs(func_id, nametable) - 1, true, nametable);
    }

    PRINT_KW_WO_NL(FUNC_CALL);
    write_var(output, tree, FUNC_NAME_NODE, VAR_NAME, nametable);
    PRINT("\n");

    // PRINT("pop rbp; push copy back\n"); // return copy
    PRINT("add rsp, %d\n", get_amount_of_parametrs(func_id, nametable) * 8);
    PRINT("XMM_PUSH xmm0; returned value\n");

    return OK;
}

err_code_t write_func_decl(FILE* output, my_tree_t* tree, node_t* node, size_t recurs_level, nametable_t nametable)
{
    PRINT("jmp ");                                     // jump func_END:
    write_var(output, tree, FUNC_NAME_NODE, VAR_NAME, nametable);
    PRINT("_END\n");

    write_var(output, tree, FUNC_NAME_NODE, VAR_NAME, nametable);
    PRINT(":\n");

    char* func_name = FUNC_NAME_BY_NODE(node);
    size_t func_id  = get_element_index(nametable, func_name);
    size_t func_above_id = get_current_func(node->parent, nametable);

    size_t local_vars = get_amount_of_local_vars_in_func(func_id, nametable)
                      - get_amount_of_parametrs         (func_id, nametable);
    PRINT("push rbp             ; save a copy rbp\n"); // copy
    PRINT("mov rbp, rsp\n"
          "sub rsp, %d\n", local_vars * 8)

    write_to_assembler(output, tree, SCOPE_EXPRESSION, recurs_level + 1, nametable);

    write_var(output, tree, FUNC_NAME_NODE, VAR_NAME, nametable); // func_END:
    PRINT("_END:\n\n");

    return OK;
}

err_code_t write_parametrs(FILE* output, my_tree_t* tree, node_t* node, int recurs_level, bool is_memory, nametable_t nametable)
{
    if (node->left  != NULL && is_memory == false) write_expression(output, tree, node->left, nametable);
    if (node->left  != NULL && is_memory == true ) {//PRINT("pop qword [rbp - %d]\n", recurs_level * 8);
                                                    }
    if (node->right != NULL) write_parametrs(output, tree, node->right, recurs_level - 1, is_memory, nametable);

    return OK;
}
