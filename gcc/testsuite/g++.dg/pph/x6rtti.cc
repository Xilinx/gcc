// { dg-xfail-if "rtti problems" { *-*-* } { "-fpph-map=pph.map" } }
// { dg-excess-errors "operator match problems due to type merging." }
// FIXME pph - We should make this a run test.

#include "x5rtti1.h"

int main()
{
    return poly1() != nonp1() && hpol1() == hnpl1();
}
