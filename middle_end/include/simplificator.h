#ifndef SIMPLIFICATOR_H_
#define SIMPLIFICATOR_H_

#include "my_tree.h"
#include "utils.h"

err_code_t reduce_equation(my_tree_t* to_reduce);
node_t* simplify_tree(my_tree_t* tree, node_t* node, bool* is_changed);

node_t* constant_folding(my_tree_t* tree, node_t* node, bool* is_changed);

node_t* mul_1_folding(my_tree_t* tree, node_t* node, bool* is_changed, tree_val_t num_to_cmp);
node_t* mul_0_folding(my_tree_t* tree, node_t* node, bool* is_changed, tree_val_t to_what_to_change);
// node_t* pow_0_folding(my_tree_t* tree, node_t* node, bool* is_changed);
node_t* negative_1_folding(my_tree_t* tree, node_t* node, bool* is_changed, tree_val_t num_to_cmp);

#endif // SIMPLIFICATOR_H_
