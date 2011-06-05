// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x1tmplclass.h:14:17: internal compiler error: Segmentation fault" "" { xfail *-*-* } 0 }
// { dg-prune-output "In file included from " }

#include "x1tmplclass.h"

int wrapper<char>::cache = 2;

template
struct wrapper<short>;

template
long wrapper<long>::cache;

int main() {
  wrapper<char> vc;
  wrapper<short> vs;
  wrapper<int> vi;
  return 0;
}
