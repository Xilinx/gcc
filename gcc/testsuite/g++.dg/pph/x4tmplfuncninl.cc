// { dg-xfail-if "ICE CGRAPH" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x4tmplfuncninl.cc:1:0: internal compiler error: in cgraph_create_node, at cgraph.c:502" "" { xfail *-*-* } 0 }

#include "x0tmplfuncninl1.h"
#include "x0tmplfuncninl2.h"
#include "a0tmplfuncninl_u.h"
