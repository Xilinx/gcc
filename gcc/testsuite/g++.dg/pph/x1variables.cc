// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "c1variables.h:5:8: error: 'int D::mbr_uninit_plain' is not a static member of 'struct D'" "" { xfail *-*-* } 0 }
// { dg-bogus "c1variables.h:6:14: error: 'const int D::mbr_init_const' is not a static member of 'struct D'" "" { xfail *-*-* } 0 }

#include "x1variables.h"

int D::mbr_init_plain = 4;
int D::mbr_uninit_plain;
const int D::mbr_init_const = 5;
