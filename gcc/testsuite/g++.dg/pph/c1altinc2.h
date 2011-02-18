#ifndef C1ALTINC2
#define C1ALTINC2
extern void foo (void);
#define X	1
#if defined X
#include "c1altinc1.h"
extern int var1;
extern int var2;
#else
#include "c1altinc1.h"
#endif
#endif
