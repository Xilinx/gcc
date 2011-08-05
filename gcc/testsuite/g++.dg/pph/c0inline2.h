#ifndef C0INLINE2_H
#define C0INLINE2_H
#include "a0inline.h"
inline int h() { struct S s = { 3, 4 }; return f(s) * 3; }
#endif
