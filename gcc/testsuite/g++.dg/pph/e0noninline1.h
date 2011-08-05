#ifndef E0NONINLINE1_H
#define E0NONINLINE1_H
#include "a0noninline.h"
int g() { struct S s = { 1, 2 }; return f(s) * 2; }
#endif
