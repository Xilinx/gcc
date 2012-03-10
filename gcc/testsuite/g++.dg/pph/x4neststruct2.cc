// {    xfail-if "NESTED TYPE MERGE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "a0neststruct.cci:3:13: error: invalid use of incomplete type .struct outer::inner" "" { xfail *-*-* } 0 }
// { dg-bogus "a0neststruct1.h:6:12: error: forward declaration of .struct outer::inner" "" { xfail *-*-* } 0 }

#include "x0neststruct2.h"
#include "x0neststruct1.h"
#include "a0neststruct.cci"
