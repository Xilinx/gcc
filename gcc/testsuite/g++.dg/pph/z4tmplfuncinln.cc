// { dg-xfail-if "ICE CGRAPH" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "z4tmplfuncinln.cc:1:0: internal compiler error: in cgraph_create_node, at cgraph.c:502" "" { xfail *-*-* } 0 }

#include "x0tmplfuncinln3.h"
#include "x0tmplfuncinln4.h"
#include "a0tmplfuncinln_u.h"
