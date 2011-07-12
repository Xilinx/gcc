#ifndef C3ALTINC2
#define C3ALTINC2
extern void foo (void);
#define X	1
#if defined X
#include "c0altinc1.h"
extern int var1;
extern int var2;
#else
#include "c0altinc1.h"
#endif
#endif
