// pph asm xdiff 37711
// xfail BOGUS MISSVAR , ORDER
// The assembly is missing a template class static member variable
// base<char>::variable instantiated in a prior pph file.
// Explicit specializations base<short>::variable and base<double>::d_variable
// appear in a different order.

#include "x0tmplclass23.h"
#include "a0tmplclass2_u.h"
