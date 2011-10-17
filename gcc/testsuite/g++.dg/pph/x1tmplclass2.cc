// pph asm xdiff 34237
// xfail BOGUS MISSVAR, MISSFUNC
// The assembly is missing a template class static member variable
// base<char>::variable instantiated in a prior pph file.
// Functions base<double>::d_static_non_inline() and
// base<double>::d_dynamic_non_inline() are also missing.

#include "x0tmplclass23.h"
#include "a0tmplclass2_u.h"
