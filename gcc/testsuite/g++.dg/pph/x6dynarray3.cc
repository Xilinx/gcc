// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x6dynarray3.cc:1:0: internal compiler error: in chainon, at tree.c" "" { xfail *-*-* } 0 }

#include "x5dynarray3.h"

#include "a0integer.h"
typedef tst::dynarray< integer > integer_array;

int main()
{
    #include "a0dynarray-use1.cci"
    #include "a0dynarray-use2.cci"
    #include "a0dynarray-use3a.cci"
    #include "a0dynarray-use4a.cci"
    return sum - 25;
}
