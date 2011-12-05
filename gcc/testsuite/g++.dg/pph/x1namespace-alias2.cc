// { dg-xfail-if "ICE build_int_cst_wide - bad merged type" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "internal compiler error: in build_int_cst_wide, at tree.c" "" { xfail *-*-* } 0 }
#include "x1namespace-alias1.h"
#include "x1namespace-alias2.h"

int g(foo::C2 c)
{
  return c.fld * 2;
}

int h(bar::C1 c)
{
  return c.another_fld - 10;
}

int f(bar::C2 c)
{
  bar::C1 x;
  return c.fld + g(c) - h(x);
}
