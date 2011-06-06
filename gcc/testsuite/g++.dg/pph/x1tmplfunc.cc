// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x1tmplfunc.h:8:16: internal compiler error: Segmentation fault" "" { xfail *-*-* } 0 }
// { dg-prune-output "In file included from " }

#include "x1tmplfunc.h"

type val = 3;

template<>
int identity< type >(type arg)
{ return arg + val; }

template
short identity(short arg);

int main() {
  return identity( 'a' );
}
