/* { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } } */
// { dg-bogus "internal compiler error: Segmentation fault" "" { xfail *-*-* } 0 }
// { dg-prune-output "In file included from" }
// pph asm xdiff
#include "x1template.h"

int x = 3;

int y = 4;

int D::method()
{ return y; }

int main()
{ }
