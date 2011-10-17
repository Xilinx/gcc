// { dg-xfail-if "ICE CGRAPH" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x4tmplclass1.cc:1:0: internal compiler error: in cgraph_create_node, at cgraph.c:502" "" { xfail *-*-* } 0 }

// Previoiusly:
// Assembly differences seem to be due to the order in which the
// symbols in the template hash tables are emitted.
#include "x0tmplclass11.h"
#include "x0tmplclass12.h"
#include "a0tmplclass1_u.h"
