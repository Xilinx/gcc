#ifndef X1STRUCT1_H
#define X1STRUCT1_H
#include "x0struct1.h"
struct D : public B {
    int method();
    int another()
    { return fld + mbr; }
    int fld;
    static int mbr;
};
#endif
