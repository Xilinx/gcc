#include "c1functions.h"
int extern_then_body(int i)             // need body, merge head to body
{ return extern_only( i ); }
static int fwdref_static(int);
int fwdref_static(int i)
{ return extern_then_body( i ); }       // need body, merge head to body
