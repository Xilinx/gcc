// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "a0incomplete2.hi:7:5: internal compiler error: in gimple_expand_cfg, at cfgexpand.c:4454" "" { xfail *-*-* } 0 }

#include "x0incomplete3.h"
#include "x0incomplete2.h"
#include "x0incomplete1.h"
#include "a0incomplete4.cci"
