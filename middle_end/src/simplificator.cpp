#include "simplificator.h"
#include <assert.h>

err_code_t reduce_equation(my_tree_t* to_reduce)
{
    assert(to_reduce);

    bool is_changed = true;
    while (is_changed)
    {
        is_changed = false;
        // TREE_DUMP(to_reduce, to_reduce->root, "Before reducing in reduce function. Size = %zu", to_reduce->size);
        to_reduce->root = simplify_tree(to_reduce, to_reduce->root, &is_changed);
        // TREE_DUMP(to_reduce, to_reduce->root, "Now this is reduced tree");
    }

    return OK;
}

node_t* simplify_tree(my_tree_t* tree, node_t* node, bool* is_changed)
{
    assert(tree);
    assert(node);
    assert(is_changed);

    if (node->right != NULL) node->right = simplify_tree(tree, node->right, is_changed);
    if (node->left  != NULL) node->left  = simplify_tree(tree, node->left , is_changed);

    if (subtree_var_count(tree, node) == ZERO_VAR && node->type == OP)
    {
        return constant_folding(tree, node, is_changed);
    }

    if (node->type == OP && (int) node->data == MUL
        && ( node->right->type == NUM && is_double_equal(node->right->data, 0)
          || node->left->type  == NUM && is_double_equal(node->left->data,  0)))
    {
        // TREE_DUMP(tree, node, "something will happened mul0");
        return  mul_0_folding(tree, node, is_changed, 0);
    }

    if (node->type == OP && (int) node->data == MUL
        && ( node->right->type == NUM && is_double_equal(node->right->data, 1)
          || node->left->type  == NUM && is_double_equal(node->left->data,  1)))
    {
        // TREE_DUMP(tree, node, "something will happened mul1");
        return mul_1_folding(tree, node, is_changed, 1);
    }

    if (node->type == OP && (int) node->data == EXP
        && ( node->right->type == NUM && is_double_equal(node->right->data, 1)))
    {
        // TREE_DUMP(tree, node, "something will happened exp^1");
        return mul_1_folding(tree, node, is_changed, 1); // not error that mul; same as * 1 == ^ 1
    }

    if (node->type == OP && (int) node->data == EXP
        && ( node->left->type == NUM && is_double_equal(node->left->data, 1)))
    {
        // TREE_DUMP(tree, node, "something will happened 1^exp");
        return mul_0_folding(tree, node, is_changed, 1); // not error that mul; same as 1 ^ == replace to 1
    }

    if (node->type == OP && (int) node->data == EXP
        && (node->right->type == NUM && is_double_equal(node->right->data, 0)))
    {
        // TREE_DUMP(tree, node, "something will happened x^0");
        return mul_0_folding(tree, node, is_changed, 1); // change to 1
    }

    if (node->type == OP && (int) node->data == EXP
        && (node->left->type == NUM && is_double_equal(node->left->data, 0)))
    {
        // TREE_DUMP(tree, node, "something will happened 0 ^");
        return mul_0_folding(tree, node, is_changed, 0); // 0 ^ ... = change to 0
    }

    if (node->type == OP && (int) node->data == ADD
        && ( node->right->type == NUM && is_double_equal(node->right->data, 0)
          || node->left->type  == NUM && is_double_equal(node->left->data,  0)))
    {
        // TREE_DUMP(tree, node, "something will happened +0 0+");
        return mul_1_folding(tree, node, is_changed, 0);
    }

    if (node->type == OP && (int) node->data == DIV
        && ( node->right->type == NUM && is_double_equal(node->right->data, 1)))
    {
        // TREE_DUMP(tree, node, "something will happened / 1");
        return mul_1_folding(tree, node, is_changed, 1);
    }

    if (node->type == OP && (int) node->data == DIV
        && ( node->left->type == NUM && is_double_equal(node->left->data, 0)))
    {
        // TREE_DUMP(tree, node, "something will happened 0 /");
        return mul_0_folding(tree, node, is_changed, 0);
    }

    if (node->type == OP && (int) node->data == SUB
        && ( node->right->type == NUM && is_double_equal(node->right->data, 0)))
    {
        // TREE_DUMP(tree, node, "something will happened -0");
        return mul_1_folding(tree, node, is_changed, 0);
    }

    if (node->type == OP && (int) node->data == SUB
        && ( node->left->type == NUM && is_double_equal(node->left->data, 0)))
    {
        TREE_DUMP(tree, node, "something will happened -0");
        return negative_1_folding(tree, node, is_changed, 0);
    }

    return node;
}

#define LEFT node->left
#define RIGHT node->right
#define ADD_(L, R) evaluate_tree(L) + evaluate_tree(R);
#define SUB_(L, R) evaluate_tree(L) - evaluate_tree(R);
#define DIV_(L, R) evaluate_tree(L) / evaluate_tree(R);
#define MUL_(L, R) evaluate_tree(L) * evaluate_tree(R);

#define EXP_(L, R) pow(evaluate_tree(L), evaluate_tree(R));
#define LOG_(L   ) log(evaluate_tree(L));

#define SIN_(L   ) sin(evaluate_tree(L));
#define COS_(L   ) cos(evaluate_tree(L));
#define TAN_(L   ) tan(evaluate_tree(L));
#define CTG_(L   ) 1 / tan(evaluate_tree(L));

#define SHN_(L   ) sinh(evaluate_tree(L));
#define CHS_(L   ) cosh(evaluate_tree(L));
#define TGH_(L   ) tanh(evaluate_tree(L));
#define CTH_(L   ) tanh(1 / evaluate_tree(L));

#define ARCSIN_(L   ) asin(evaluate_tree(L));
#define ARCCOS_(L   ) acos(evaluate_tree(L));
#define ARCTAN_(L   ) atan(evaluate_tree(L));
#define ARCCTG_(L   ) M_PI / 2 - atan(evaluate_tree(L));

#define ARCSHN_(L   ) asinh(evaluate_tree(L));
#define ARCCHS_(L   ) acosh(evaluate_tree(L));
#define ARCTGH_(L   ) atanh(evaluate_tree(L));
#define ARCCTH_(L   ) atanh(1 / evaluate_tree(L));

tree_val_t evaluate_tree(node_t* node)
{
    if (node->type == NUM)     return node->data;
    if (node->type == VAR)     return 1;
    if (node->type == SUBTREE) return evaluate_tree(node->left);

    switch ((int) node->data)
    {
        case ADD: return ADD_(LEFT, RIGHT);
        case SUB: return SUB_(LEFT, RIGHT);
        case DIV: return DIV_(LEFT, RIGHT);
        case MUL: return MUL_(LEFT, RIGHT);

        case SIN: return SIN_(LEFT);
        case COS: return COS_(LEFT);
        case TAN: return TAN_(LEFT);
        case CTG: return CTG_(LEFT);

        case SHN: return SHN_(LEFT);
        case CHS: return CHS_(LEFT);
        case TGH: return TGH_(LEFT);
        case CTH: return CTH_(LEFT);

        case EXP: return EXP_(LEFT, RIGHT);
        case LOG: return LOG_(LEFT);

        case ARCSIN: return ARCSIN_(LEFT);
        case ARCCOS: return ARCCOS_(LEFT);
        case ARCTAN: return ARCTAN_(LEFT);
        case ARCCTG: return ARCCTG_(LEFT);

        case ARCSHN: return ARCSHN_(LEFT);
        case ARCCHS: return ARCCHS_(LEFT);
        case ARCTGH: return ARCTGH_(LEFT);
        case ARCCTH: return ARCCTH_(LEFT);


        default: fprintf(stderr, "Unknown operation in evaluate\n"); return 0;
    }
}
#undef ADD_
#undef SUB_
#undef DIV_
#undef MUL_
#undef EXP_
#undef LEFT
#undef RIGHT

node_t* constant_folding(my_tree_t* tree, node_t* node, bool* is_changed)
{
    assert(tree);
    assert(node);
    assert(is_changed);

    node_t* to_ret = NULL;
    // TREE_DUMP(tree, node, "we are here calculating this");
    *is_changed = true;
    tree_val_t new_value = evaluate_tree(node);
    to_ret = new_node(tree, NUM, new_value, NULL, NULL);
    to_ret->parent = node->parent;

    if (node == tree->root) tree->root = to_ret;
    node_dtor(node);

    return to_ret;
}

node_t* mul_1_folding(my_tree_t* tree, node_t* node, bool* is_changed, tree_val_t num_to_cmp)
{
    assert(tree);
    assert(node);
    assert(is_changed);

    node_t* to_ret = NULL;
    *is_changed = true;
    if      (node->left->type == NUM // TODO: macros to check type and value
        &&   is_double_equal(node->left->data,  num_to_cmp)) to_ret = copy_subtree(tree, node->right);
    else if (node->right->type == NUM
        &&   is_double_equal(node->right->data, num_to_cmp)) to_ret = copy_subtree(tree, node->left); // TODO: optimize deleting

    if (node == tree->root) tree->root = to_ret;
    to_ret->parent = node->parent;
    // TREE_DUMP(tree, node, "replacing this node with input data = %lf", num_to_cmp);
    node_dtor(node);

    return to_ret;
}

node_t* mul_0_folding(my_tree_t* tree, node_t* node, bool* is_changed, tree_val_t to_what_to_change)
{
    assert(tree);
    assert(node);
    assert(is_changed);

    node_t* to_ret = NULL;
    *is_changed = true;
    // TREE_DUMP(tree, node, "gonna changing this to zero this");
    to_ret = new_node(tree, NUM, to_what_to_change, NULL, NULL);
    if (node == tree->root) tree->root = to_ret;

    to_ret->parent = node->parent;
    node_dtor(node);

    return to_ret;
}

node_t* negative_1_folding(my_tree_t* tree, node_t* node, bool* is_changed, tree_val_t num_to_cmp)
{
    assert(tree);
    assert(node);
    assert(is_changed);

    node_t* to_ret = NULL;
    *is_changed = true;
    // TREE_DUMP(tree, node, "gonna changing this to zero this");
    node_t* neg_1 = new_node(tree, NUM, -1, NULL, NULL);
    node_t* expr  = copy_subtree(tree, node->right);
    to_ret = new_node(tree, OP, MUL, neg_1, expr);
    neg_1->parent = expr->parent = to_ret;

    if (node == tree->root) tree->root = to_ret;

    to_ret->parent = node->parent;
    node_dtor(node);

    return to_ret;
}
