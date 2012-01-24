// pph asm xdiff 37711
// xfail BOGUS MISSVAR
// The assembly is missing a template class static member variable
// base<char>::variable instantiated in a prior pph file.
// Others symbols are emitted in a different order.
#include "x0tmplclass23.h"
#include "a0tmplclass2_u.h"
