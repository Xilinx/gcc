#ifndef X1STRUCT_H
#define X1STRUCT_H
#include "c1struct.h"
struct D : thing {
    type method();
    type another()
    { return fld + mbr + gbl; }
    type fld;
    static type mbr;
};
#endif
