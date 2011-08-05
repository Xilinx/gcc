#ifndef E0NONINLINE2_H
#define E0NONINLINE2_H
#include "a0noninline.h"
int h() { struct S s = { 3, 4 }; return f(s) * 3; }
#endif
