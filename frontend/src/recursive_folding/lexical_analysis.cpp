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

    // fread(temp_buf, 1, filesize, input_file);
    for (size_t i = 0; i < filesize; i++)
    {
        if (fscanf(input_file, "%c", temp_buf++) == EOF)
        {
            break;
        }
    }
    fclose(input_file);

    return OK;
}

size_t lexical_analysis(tokens* token, char* buffer)
{
    size_t pos = 0;
    char* end_pos = buffer;
    size_t token_index = 0;
    size_t lines  = 1;
    size_t column = 1;

    while ((*end_pos) != '\0')
    {
        if (token_index >= MAXIMUM_LEXEMS_COUNT - 1)
        {
            assert("You should increase MAXIMUM_LEXEMS_COUNT" == NULL);
        }
        printf("current char is %c, addr = %p\n", *end_pos, end_pos);
        if (isdigit(*end_pos))
        {
            char* begin = end_pos;
            tree_val_t value = strtod(end_pos, &end_pos);
            token[token_index].value = value;
            token[token_index].type  = NUM;
            printf("scanned value is %lg\n", value);
            end_pos--;
            column += end_pos - begin;
            // printf("buffer = %p, end_pos = %p, diff = %zu\n", buffer, end_pos, end_pos - buffer);
            token_index++;
        }
        else if (isalpha(*end_pos))
        {
            char* begin = end_pos;
            while (isalnum(*end_pos) || *end_pos == '_')
            {
                end_pos++;
            }
            column += end_pos - begin;

            size_t key_word = is_key_word(begin, end_pos);
            printf("key word is %zu\n", key_word);
            if(key_word == UNKNOWN)
            {
                // tokens[token_index].value = ;
                // TODO: increase more than 8 bytes
                // memcpy(&tokens[token_index].value, begin, end_pos - begin);
                // printf("Var_name = %8s\n", tokens[token_index].value);
                if (end_pos - begin >= 8)
                {
                    assert("Var name should be shorter 8 symbols" == NULL);
                }
                printf("Var_name: End_pos = %p, begin = %p, diff = %zu\n", end_pos, begin, end_pos - begin);
                char* var_name = (char*) calloc(sizeof(tree_val_t), sizeof(char));
                strncpy(var_name, begin, end_pos - begin);

                memcpy(&token[token_index].value, &var_name, sizeof(tree_val_t));
                // printf();
                token[token_index].type  = VAR;
                token_index++;
            }
            else
            {
                token[token_index].value = key_word;
                token[token_index].type  = key_word > STATEMENT_BEGIN ? STATEMENT: OP;
                token_index++;
            }
            end_pos--;
        }
        else
        {
            column += 1;
            size_t key_word = is_key_word(end_pos, end_pos + 1);

            if (key_word == UNKNOWN)
            {
                printf("Something unknown opreation = %c\n", *end_pos);
                abort();
            }
            token[token_index].value = key_word;
            token[token_index].type  = key_word > STATEMENT_BEGIN ? STATEMENT: OP;
            token_index++;
        }

        token[token_index].line   = lines;
        token[token_index].column = column;

        end_pos += 1;

        skip_spaces(&end_pos, &column, &lines);
    }

    token[token_index].type  = END;
    token[token_index].value = '$';
    token_index++;
    printf("\n");

    return token_index;
}

err_code_t skip_spaces(char** input, size_t* column, size_t* lines)
{
    printf("symbol to skip: %d\n", (int) **input);
    while (isblank(**input) || **input == '\n')
    {
        printf("symbol to skip: %d\n", (int) **input);

        (*column)++;
        if (**input == '\n')
        {
            printf("Increasing lines\n");
            (*column) = 1;
            (*lines)++;
        }
        (*input)++;
    }

    return OK;
}

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
        if (!strncmp(all_ops[i].text, begin, end - begin) && strlen(all_ops[i].text) == end - begin)
        {
            printf("Finded statement is %s. Index = %zu\n", all_ops[i].text, i);
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
            printf("index = %02zu, type = OP , name = %s", i,
                                                     all_ops[(int) programm_tokens[i].value].text);
        }
        else if (programm_tokens[i].type == NUM)
        {
            printf("index = %02zu, type = NUM, name = %lg", i,
                        programm_tokens[i].value);
        }
        else if (programm_tokens[i].type == VAR)
        {
            printf("index = %02zu, type = VAR, name = %s", i,
                                    *(char**) &programm_tokens[i].value);
        }
        if (programm_tokens[i].type == STATEMENT)
        {
            printf("index = %02zu, type = STA, name = %s", i,
                                                     all_ops[(int) programm_tokens[i].value].text);
        }
        else if (programm_tokens[i].type == END)
        {
            printf("index = %02zu, type = END, name = %c", i,
                        (char) programm_tokens[i].value);
        }

        printf(" line = %zu, column = %zu\n", programm_tokens[i].line, programm_tokens[i].column);
    }

    return OK;
}

err_code_t free_tokens(tokens* programm_tokens, size_t tokens_num)
{
    for (size_t i = 0; i < tokens_num; i++)
    {
        if (programm_tokens[i].type == VAR)
        {
            // free(*(char**) &programm_tokens[i].value);
        }
    }
    free(programm_tokens);

    return OK;
}
