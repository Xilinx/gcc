// { dg-xfail-if "ICE CGRAPH" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "z4tmplclass2.cc:1:0: internal compiler error: in cgraph_create_node, at cgraph.c:502" "" { xfail *-*-* } 0 }

// Previously
// xfail BOGUS DUPVAR DUPFUNC

#include "x0tmplclass23.h"
#include "x0tmplclass24.h"
#include "a0tmplclass2_u.h"
