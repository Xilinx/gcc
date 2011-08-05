#ifndef C0INLINE1_H
#define C0INLINE1_H
#include "a0inline.h"
inline int g() { struct S s = { 1, 2 }; return f(s) * 2; }
#endif
