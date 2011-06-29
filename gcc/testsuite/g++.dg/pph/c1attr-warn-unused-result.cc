/* { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } } */
// { dg-bogus "internal compiler error: Segmentation fault" "" { xfail *-*-* } 0 }
// { dg-prune-output "In file included from" }
/* { dg-options "-w" } */
// pph asm xdiff
#include "c1attr-warn-unused-result.h"
