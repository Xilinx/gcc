// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x0tmplclass.h:14:5: error: specializing member .wrapper<char>::cache. requires .template<>. syntax" "" { xfail *-*-* } 0 }

#include "x0tmplclass.h"

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
