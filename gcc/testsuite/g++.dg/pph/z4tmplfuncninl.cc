// { dg-xfail-if "ICE instantiate_decl - bad merge" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "internal compiler error: in instantiate_decl, at cp/pt.c" "" { xfail *-*-* } 0 }
// { dg-excess-errors "decl merge problems" { xfail *-*-* } }
#include "x0tmplfuncninl3.h"
#include "x0tmplfuncninl4.h"
#include "a0tmplfuncninl_u.h"
