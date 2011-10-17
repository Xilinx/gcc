#ifndef X1OVERSET1_H
#define X1OVERSET1_H

#include "x0overset0.h"

int func(base1 *x);
int func(base2 *x);

inline int overloader1()
{
    derived1 v1;
    derived2 v2;
    return func(&v1) + func(&v2);
}

#endif
