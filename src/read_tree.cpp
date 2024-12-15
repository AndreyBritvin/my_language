#include "read_tree.h"
#include <assert.h>
#include "utils.h"
#include <stdio.h>
#include "my_tree.h"
#include <sys/stat.h>
#include <string.h>
#include "name_table.h"

size_t get_file_len(const char *filename)
{
    assert(filename != NULL);

    struct stat st = {};
    stat(filename, &st);

    return (size_t) st.st_size;
}

err_code_t fill_buffer(char **buffer_to_fill, const char* filename)
{
    size_t filesize = get_file_len(filename);
    char *temp_buf = (char *) calloc(filesize + 1, sizeof(char));
    if (temp_buf == NULL)
    {
        return ERROR_CALLOC_IS_NULL;
    }
    *buffer_to_fill = temp_buf;

    FILE* SAFE_OPEN_FILE(input_file, filename, "r");

    fread(temp_buf, 1, filesize, input_file);
    // printf("First symbol %c\n", temp_buf[0]);
    // printf("Readed value = %s\n", temp_buf);
    fclose(input_file);

    return OK;
}

my_tree_t make_prog_tree(char *buffer, nametable_t nametable)
{
    assert(buffer);

    INIT_TREE(tree_to_fill);

    free(tree_to_fill.root);

    size_t position = 0;

    tree_to_fill.root = fill_node(buffer + check_signature(buffer), &position, &tree_to_fill, NULL, nametable);
    TREE_DUMP(&tree_to_fill, tree_to_fill.root, "I am gROOT (generated this tree after reading file)");
    get_func_dependence(nametable);

    return tree_to_fill;
}

size_t check_signature(char* input)
{
    char buffer[20] = {};

    return 16; // TODO: check signature and give true offset
}

node_t* fill_node(char * buffer, size_t* position, my_tree_t* tree, node_t* parent, nametable_t nametable)
{
    assert(buffer);
    assert(position);
    assert(tree);

    (*position)++;
    // char *expression = (char *) calloc(128, sizeof(char));
    char* expression = (char *) calloc(MAX_STRING_SIZE, sizeof(char));
    if (expression == NULL)
    {
        fprintf(stderr, "Ended memory in fill_node in calloc\n");
        return NULL;
    }

    size_t len_of_expr = 0;
    char expr_type[4] = {};
    sscanf(buffer + *position, "%[^:]:\"%[^\"]\"%n", expr_type, expression, &len_of_expr);
    *position += len_of_expr;

    tree->size++;

    int type = END;
    int func_num = get_func_num(expression);
    tree_val_t number_value = 0;

    if (func_num < STATEMENT_BEGIN) // TODO: make func get_type
    {
        type = OP;
    }
    else if (func_num > STATEMENT_BEGIN && func_num != UNKNOWN)
    {
        type = STATEMENT;
    }
    else if (func_num == UNKNOWN)
    {
        if (sscanf(expression, "%lg", &number_value) == 0) type = VAR;
        else                                               type = NUM;
    }

    printf("Readed expression is %8s. Type = %d\n", expression, type);

    node_t* node_to_return = NULL; // TODO: make func to generate node
    if (type == NUM)
    {
        node_to_return = new_node(tree, type, number_value, NULL, NULL);
    }
    else if (type == STATEMENT || type == OP)
    {
        node_to_return = new_node(tree, type, func_num, NULL, NULL);
    }
    else if (type == VAR)
    {
        node_to_return = new_node(tree, type, '\0', NULL, NULL);
        memcpy(&node_to_return->data, &expression, sizeof(tree_val_t));
    }
    if (type != VAR)
    {
        free(expression);
    }

    if (node_to_return != NULL)
    {
        node_to_return->parent = parent;
        //TODO: make func to add to nametable
        if (type == VAR && is_element_in_nt(nametable, expression) == MAX_ID_COUNT)
        {
            identificator id = {};
            id.dependence = MAX_ID_COUNT;
            strcpy(id.name, expression);
            id.length = strlen(expression);

            if (parent->type == STATEMENT && (int) parent->data == EQUAL_BEGIN)
            {
                id.is_defined = true; // TODO: check if not right subtree
            }
            if (parent->type == STATEMENT && (int) parent->data == SEPARATOR)
            {
                id.type = PARAM_TYPE;
            }
                // maybe fix for local vars
            id.node_dep = get_func_node_dependence(nametable, parent);

            if (parent->type == STATEMENT && (int) parent->data == FUNC_SPEC)
            {
                id.type = FUNC_TYPE;

                if (parent->parent->type == STATEMENT && (int) parent->parent->data == FUNC_DECL)
                {
                    id.is_defined = true;
                }
            }

            add_element(nametable, id);
        }
    }

    if (*position == 1)
    {
        tree->root = node_to_return;
    }

    bool is_left = true;

    while (buffer[*position] != '}')
    {
        // TREE_DUMP(tree, node_to_return, "Working with this\n Input text = %s", expression);
        // printf("Current char is %c position %zu\n", buffer[*position], *position);
        if (buffer[*position] == '{' && is_left)
        {
            is_left = false;
            node_to_return->left = fill_node(buffer, position, tree, node_to_return, nametable);
        }

        else if (buffer[*position] == '{' && !is_left)
        {
            node_to_return->right = fill_node(buffer, position, tree, node_to_return, nametable);
        }

        (*position)++;
    }

    return node_to_return;
}

err_code_t get_func_dependence(nametable_t nt)
{
    for (size_t i = 0; i < MAX_ID_COUNT; i++)
    {
        if (nt[i].node_dep != NULL)
        {
            char* func_name = *(char**)&nt[i].node_dep->left->left->data;
            nt[i].is_defined = true;
            nt[i].dependence = is_element_in_nt(nt, func_name);
        }
    }

    return OK;
}

node_t* get_func_node_dependence(nametable_t nt, node_t* dep_node)
{
    if (dep_node->type == STATEMENT && (int) dep_node->data == FUNC_DECL)
    {
        return dep_node;
    }
    if (dep_node->parent != NULL) return get_func_node_dependence(nt, dep_node->parent);

    return NULL;
}

int get_func_num(char* input)
{
    // printf("%s\n", input);
    // printf("All_ops size = %d\n", sizeof(all_ops));

    for (size_t i = 0; i < sizeof(all_ops) / sizeof(operation); i++)
    {
        if (!strcmp(all_ops[i].standart_text, input))
        {
            return i;
        }
    }

    return UNKNOWN;
}
