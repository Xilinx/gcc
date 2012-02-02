// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "internal compiler error: in import_export_decl, at cp/decl2.c" "" { xfail *-*-* } 0 }

#include "x1incomplete3.h"
#include "a0incomplete4.cci"
