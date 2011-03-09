#ifndef __CFLOW_H
#define __CFLOW_H

extern void foo (void);

#define X	1
#if defined X
#include "inif.h"
extern int var1;
extern int var2;
#endif
#include "inif.h"
#endif
