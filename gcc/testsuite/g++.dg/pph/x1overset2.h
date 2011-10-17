#ifndef X1OVERSET2_H
#define X1OVERSET2_H

#include "x0overset0.h"

int func(derived1 *x);
int func(derived2 *x);

inline int overloader2()
{
    derived1 v1;
    derived2 v2;
    return func(&v1) + func(&v2);
}

#endif
