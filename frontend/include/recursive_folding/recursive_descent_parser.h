#ifndef RECURSIVE_DESCNET_PARSER_H_
#define RECURSIVE_DESCNET_PARSER_H_

#include "stdlib.h"
#include "my_tree.h"
#include "lexical_analysis.h"

typedef node_t* (syntax_analysis_func_t)(my_tree_t* tree, tokens* input, size_t* pos);

my_tree_t get_grammatic(tokens* input);
bool check_return_to_the_end(my_tree_t* tree, node_t* where_to_connect);

syntax_analysis_func_t get_number     ;
syntax_analysis_func_t get_expression ;
syntax_analysis_func_t get_mul_div    ;
syntax_analysis_func_t get_exp        ;
syntax_analysis_func_t get_primary    ;
syntax_analysis_func_t get_variable   ;
syntax_analysis_func_t get_func       ;
syntax_analysis_func_t get_assingnment;
syntax_analysis_func_t get_statement  ;
syntax_analysis_func_t get_if_state   ;
syntax_analysis_func_t get_while_state;
syntax_analysis_func_t get_print_state;
syntax_analysis_func_t get_comparison ;
syntax_analysis_func_t get_func_decl  ;
syntax_analysis_func_t get_return     ;
syntax_analysis_func_t get_func_call  ;

#endif // RECURSIVE_DESCNET_PARSER_H_
