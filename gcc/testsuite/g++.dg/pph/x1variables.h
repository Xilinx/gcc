#ifndef X1VARIABLES_H
#define X1VARIABLES_H
#include "c0variables1.h"
struct D {
    static int mbr_init_plain;
    static int mbr_uninit_plain;
    static const int mbr_init_const;
    static const int mbr_uninit_const;
    static const int mbr_manifest = 3;
};
#endif
