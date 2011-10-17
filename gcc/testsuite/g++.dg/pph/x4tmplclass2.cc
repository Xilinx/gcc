// { dg-xfail-if "ICE CGRAPH" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x4tmplclass2.cc:1:0: internal compiler error: in cgraph_create_node, at cgraph.c:502" "" { xfail *-*-* } 0 }

// Prevously:
// xfail BOGUS DUPFUN
// base<short>::dynamic_early_inline() and base<short>::static_early_inline()
// are duplicated.

#include "x0tmplclass21.h"
#include "x0tmplclass22.h"
#include "a0tmplclass2_u.h"
