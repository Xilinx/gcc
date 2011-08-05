#ifndef C0RAWSTRUCT1_H
#define C0RAWSTRUCT1_H
#include "a0rawstruct.h"
int g() { struct S s = { 1, 2 }; return s.a; }
#endif
