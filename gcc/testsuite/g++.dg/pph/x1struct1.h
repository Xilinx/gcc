#ifndef X1STRUCT1_H
#define X1STRUCT1_H
#include "x1struct0.h"
struct D : public thing {
    type method();
    type another()
    { return fld + mbr; }
    type fld;
    static type mbr;
};
#endif
