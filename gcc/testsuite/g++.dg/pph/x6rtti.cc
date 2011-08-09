// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }

//FIXME We should make this a run test.

#include "x5rtti1.h"

int main()
{
    return poly1() != nonp1() && hpol1() == hnpl1();
}
