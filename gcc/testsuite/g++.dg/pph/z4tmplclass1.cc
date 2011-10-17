// { dg-xfail-if "ICE CGRAPH" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "z4tmplclass1.cc:1:0: internal compiler error: in cgraph_create_node, at cgraph.c:502" "" { xfail *-*-* } 0 }

// Previously
// xfail BOGUS DUPVAR DUPFUNC

#include "x0tmplclass13.h"
#include "x0tmplclass14.h"
#include "a0tmplclass1_u.h"
