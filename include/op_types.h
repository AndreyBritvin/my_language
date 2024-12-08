#ifndef OP_TYPES_H_
#define OP_TYPES_H_

enum node_type
{
    NUM,
    VAR,
    OP,
    SUBTREE,
    END
};

enum op_type
{
    ADD     =  0,
    SUB     =  1,
    MUL     =  2,
    DIV     =  3,
    SIN     =  4,
    COS     =  5,
    EXP     =  6,
    TAN     =  7,
    SHN     =  8,
    CHS     =  9,
    CTH     = 10,
    TGH     = 11,
    CTG     = 12,
    LOG     = 13,
    ARCSIN  = 14,
    ARCCOS  = 15,
    ARCTAN  = 16,
    ARCCTG  = 17,
    ARCSHN  = 18,
    ARCCHS  = 19,
    ARCTGH  = 20,
    ARCCTH  = 21,
    BRACKET_OPEN  = 22,
    BRACKET_CLOS  = 23,
    UNKNOWN
};

struct operation
{
    op_type      type;
    const char*  text;
};

static operation all_ops[] = {//!!! order should be equal to enum!!!
    {ADD, "+"},
    {SUB, "-"},
    {MUL, "*"},
    {DIV, "/"},
    {SIN, "sin"},
    {COS, "cos"},
    {EXP, "^"},
    {TAN, "tan"},
    {SHN, "sinh"},
    {CHS, "cosh"},
    {CTH, "coth"},
    {TGH, "tanh"},
    {CTG, "cot"},
    {LOG, "ln"},
    {ARCSIN, "arcsin"},
    {ARCCOS, "arccos"},
    {ARCTAN, "arctan"},
    {ARCCTG, "arcctg"},
    {ARCSHN, "arcsinh"},
    {ARCCHS, "arccosh"},
    {ARCTGH, "arctanh"},
    {ARCCTH, "arccoth"},
    {BRACKET_OPEN, "("},
    {BRACKET_CLOS, ")"},
};

#endif // OP_TYPES_H_
