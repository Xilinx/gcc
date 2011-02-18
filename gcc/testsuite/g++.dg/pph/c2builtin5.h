#ifndef C2BUILTIN5_H
#define C2BUILTIN5_H
#include "a2builtin4.h"
const char *file = VALUE(__FILE__);
const char *line = VALUE(__LINE__);
const char *vers = VALUE(__cplusplus);
const char *optm = VALUE(__OPTIMIZE__);
const char *func() { return __func__; }
#endif
