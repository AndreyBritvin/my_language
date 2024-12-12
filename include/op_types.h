#ifndef OP_TYPES_H_
#define OP_TYPES_H_

enum node_type
{
    NUM,
    VAR,
    OP,
    STATEMENT,
    SUBTREE,
    END
};

enum op_type
{
    FUNCTIONS = -1,
#define OPERATION(num, ...) num,
#include "types.dsl"
#undef OPERATION

    UNKNOWN
};

struct operation
{
    op_type      type;
    const char*  text;
    const char*  standart_text;
    const char*  assembler_text;
};

static operation all_ops[] = {//!!! order should be equal to enum!!! // TODO: codegen as in proc
#define OPERATION(num, ...) {num, __VA_ARGS__},
#include "types.dsl"
#undef OPERATION
};

#endif // OP_TYPES_H_
