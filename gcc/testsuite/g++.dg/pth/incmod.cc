struct T { int f; };
#define NAME v
#define VALUE 1
#include "incmod.h"
#undef NAME
#define NAME w
#undef VALUE
#define VALUE 2
#include "incmod.h"
