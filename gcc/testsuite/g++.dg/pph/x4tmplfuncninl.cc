// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "a0tmplfuncninl_g.h:12:23: internal compiler error: in instantiate_decl" "" { xfail *-*-* } 0 }
// { dg-excess-errors "Template list problems" }

#include "x0tmplfuncninl1.h"
#include "x0tmplfuncninl2.h"
#include "a0tmplfuncninl_u.h"
