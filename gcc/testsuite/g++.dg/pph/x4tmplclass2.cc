// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x4tmplclass2.cc:1:0: fatal error: LTO_tags out of range: Range is 0 to 355, value is 22275" "" { xfail *-*-* } 0 }

#include "x0tmplclass21.h"
#include "x0tmplclass22.h"
#include "a0tmplclass2_u.h"
