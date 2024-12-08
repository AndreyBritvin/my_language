#include "lexical_analysis.h"
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <ctype.h>

size_t get_file_len(const char *filename)
{
    assert(filename != NULL);

    struct stat st = {};
    stat(filename, &st);

    return (size_t) st.st_size;
}

char* fill_buffer(char **buffer_to_fill, const char* filename)
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
    fclose(input_file);

    return temp_buf;
}

size_t lexical_analysis(tokens* token, char* buffer)
{
    size_t pos = 0;
    char* end_pos = buffer;
    size_t token_index = 0;

    while ((*end_pos) != '\0' && (*end_pos) != '\n')
    {
        if (token_index >= MAXIMUM_LEXEMS_COUNT)
        {
            assert("You should increase MAXIMUM_LEXEMS_COUNT" == NULL);
        }
        printf("current char is %c, addr = %p\n", *end_pos, end_pos);
        if (isdigit(*end_pos))
        {
            tree_val_t value = strtod(end_pos, &end_pos);
            token[token_index].value = value;
            token[token_index].type  = NUM;
            printf("scanned value is %lg\n", value);
            end_pos--;
            // printf("buffer = %p, end_pos = %p, diff = %zu\n", buffer, end_pos, end_pos - buffer);
            token_index++;
        }
        else if (isalpha(*end_pos))
        {
            char* begin = end_pos;
            while (isalnum(*end_pos))
            {
                end_pos++;
            }

            size_t key_word = is_key_word(begin, end_pos);
            printf("key word is %zu\n", key_word);
            if(key_word == UNKNOWN)
            {
                // tokens[token_index].value = ;
                // TODO: increase more than 8 bytes
                // memcpy(&tokens[token_index].value, begin, end_pos - begin);
                // printf("Var_name = %8s\n", tokens[token_index].value);
                printf("Var_name: End_pos = %p, begin = %p, diff = %zu\n", end_pos, begin, end_pos - begin);
                char* var_name = (char*) calloc(end_pos - begin + 1, sizeof(char));
                strncpy(var_name, begin, end_pos - begin);
                memcpy(&token[token_index].value, &var_name, sizeof(tree_val_t));
                token[token_index].type  = VAR;
                token_index++;
            }
            else
            {
                token[token_index].value = key_word;
                token[token_index].type  = OP;
                token_index++;
            }
            end_pos--;
        }
        else
        {
            size_t key_word = is_key_word(end_pos, end_pos + 1);
            printf("Something unknown opreation = %c\n", *end_pos);
            token[token_index].value = key_word;
            token[token_index].type  = OP;
            token_index++;
        }

        end_pos += 1;
    }

    token[token_index].type  = END;
    token[token_index].value = '$';
    token_index++;
    printf("\n");

    return token_index;
}
/*
node_t* fill_node(char * buffer, size_t* position, my_tree_t* tree, node_t* parent)
{
    assert(buffer);
    assert(position);
    assert(tree);

    (*position)++;

    size_t len_of_expr = 0;
    tree_val_t expression = 0;
    char funcname[MAX_STRING_SIZE] = {};

    // printf("Type = %d ", sscanf(buffer + *position, "%[^()]%ln", &funcname, &len_of_expr));
    // printf("Get_funcname = %d\n", get_func_num(funcname));

    op_type_t var_type = NUM;
    if (sscanf(buffer + *position, "%lf%ln", &expression, &len_of_expr) > 0)
    {
        var_type = NUM;
    }
    else if (sscanf(buffer + *position, "%[^()]%ln", &funcname, &len_of_expr) >= 1
             && get_func_num(funcname) != UNKNOWN)
    {
        var_type = OP;
        expression = (tree_val_t) get_func_num(funcname);
    }
    else if (sscanf(buffer + *position, "%c%ln", &funcname, &len_of_expr))
    {
        var_type = VAR;
        expression = (tree_val_t) funcname[0];
    }
    else
    {
        fprintf(stderr, "No sscanf dont work\n");
    }

    *position += len_of_expr;

    tree->size++;
    node_t* node_to_return = new_node(tree, var_type, expression, NULL, NULL);
    node_to_return->parent = parent;
    if (*position == 1)
    {
        tree->root = node_to_return;
    }

    bool is_left = true;

    while (buffer[*position] != ')')
    {
        // TREE_DUMP(tree, node_to_return, "Working with this\n Input text = %s", expression);
        // printf("Current char is %c position %zu\n", buffer[*position], *position);
        if (buffer[*position] == '(' && is_left)
        {
            is_left = false;
            node_to_return->left = fill_node(buffer, position, tree, node_to_return);
        }

        else if (buffer[*position] == '(' && !is_left)
        {
            node_to_return->right = fill_node(buffer, position, tree, node_to_return);
        }

        else
        {
            fprintf(stderr, "Some error happened in brackets\n");
        }

        (*position)++;
    }

    return node_to_return;
}*/

int get_func_num(char* input)
{
    // printf("%s\n", input);
    // printf("All_ops size = %d\n", sizeof(all_ops));

    for (size_t i = 0; i < sizeof(all_ops) / sizeof(operation); i++)
    {
        if (!strcmp(all_ops[i].text, input))
        {
            return i;
        }
    }

    return UNKNOWN;
}

size_t is_key_word(char* begin, char* end)
{
    for (size_t i = 0; i < sizeof(all_ops) / sizeof(operation); i++)
    {
        if (!strncmp(all_ops[i].text, begin, end - begin))
        {
            return i;
        }
    }

    return UNKNOWN;
}

err_code_t printf_tokens(tokens* programm_tokens, size_t tokens_num)
{
    for (size_t i = 0; i < tokens_num; i++)
    {
        if (programm_tokens[i].type == OP)
        {
            printf("index = %02zu, type = OP , name = %s\n", i,
                                                     all_ops[(int) programm_tokens[i].value].text);
        }
        else if (programm_tokens[i].type == NUM)
        {
            printf("index = %02zu, type = NUM, name = %lg\n", i,
                        programm_tokens[i].value);
        }
        else if (programm_tokens[i].type == VAR)
        {
            printf("index = %02zu, type = VAR, name = %s\n", i,
                        *(char**) &programm_tokens[i].value);
        }
        else if (programm_tokens[i].type == END)
        {
            printf("index = %02zu, type = END, name = %c\n", i,
                        (char) programm_tokens[i].value);
        }
    }

    return OK;
}

err_code_t free_tokens(tokens* programm_tokens, size_t tokens_num)
{
    for (size_t i = 0; i < tokens_num; i++)
    {
        if (programm_tokens[i].type == VAR)
        {
            free(*(char**) &programm_tokens[i].value);
        }
    }
    free(programm_tokens);

    return OK;
}
