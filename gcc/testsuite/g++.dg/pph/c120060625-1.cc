// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "c120060625-1.h:14:22: internal compiler error: invalid built-in macro .__FLT_MAX__." "" { xfail *-*-* } 0 }
// { dg-prune-output "In file included from " }

#include "c120060625-1.h"
