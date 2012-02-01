#ifndef X1STRUCT2_H
#define X1STRUCT2_H
#include "x0struct2.h"
struct D : public thing {
    type method();
    type another()
    { return fld + mbr; }
    type fld;
    static type mbr;
};
#endif
