// { dg-xfail-if "ICE CGRAPH" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x4tmplfuncinln.cc:1:0: internal compiler error: in cgraph_create_node, at cgraph.c:502" "" { xfail *-*-* } 0 }

#include "x0tmplfuncinln1.h"
#include "x0tmplfuncinln2.h"
#include "a0tmplfuncinln_u.h"
