#ifndef RECURSIVE_DESCNET_PARSER_H_
#define RECURSIVE_DESCNET_PARSER_H_

#include "stdlib.h"
#include "my_tree.h"
#include "lexical_analysis.h"

my_tree_t get_grammatic(tokens* input);
node_t* get_number     (my_tree_t* tree, tokens* input, size_t* pos);
node_t* get_expression (my_tree_t* tree, tokens* input, size_t* pos);
node_t* get_mul_div    (my_tree_t* tree, tokens* input, size_t* pos);
node_t* get_exp        (my_tree_t* tree, tokens* input, size_t* pos);
node_t* get_primary    (my_tree_t* tree, tokens* input, size_t* pos);
node_t* get_variable   (my_tree_t* tree, tokens* input, size_t* pos);
node_t* get_func       (my_tree_t* tree, tokens* input, size_t* pos);

#endif // RECURSIVE_DESCNET_PARSER_H_
