#ifndef LEXICAL_ANALYSIS_H_
#define LEXICAL_ANALYSIS_H_

#include "op_types.h"
#include "my_tree.h"
// #include "differenciator.h"

#define MAXIMUM_LEXEMS_COUNT 500

struct tokens
{
    node_type  type;
    tree_val_t value;
    size_t     line;
    size_t     column;
};

size_t get_file_len(const char *filename);
err_code_t fill_buffer(char **buffer_to_fill, const char* filename);
my_tree_t make_tree(char *buffer);
err_code_t skip_spaces(char** input, size_t* column, size_t* lines);
node_t* fill_node(char * buffer, size_t* position, my_tree_t* tree, node_t* parent);
size_t lexical_analysis(tokens* token, char* buffer);
size_t is_key_word(char* begin, char* end);
int get_func_num(char* input);
err_code_t printf_tokens(tokens* programm_tokens, size_t tokens_num);
err_code_t free_tokens  (tokens* programm_tokens, size_t tokens_num);

#endif // LEXICAL_ANALYSIS_H_
