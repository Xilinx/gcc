// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "mathcalls.h:365:1: internal compiler error: Segmentation fault" "" { xfail *-*-* } 0 }
// { dg-prune-output "In file included from " }
// { dg-prune-output "                 from " }
// { dg-options "-w -fpermissive" }

#include "c1eabi1.h"
