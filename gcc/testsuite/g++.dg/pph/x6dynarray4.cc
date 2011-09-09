// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "internal compiler error: Segmentation fault" "" { xfail *-*-* } 0 }
#include "x6dynarray5.h"

#include <algorithm>

#include "a0integer.h"
typedef tst::dynarray< integer > integer_array;

int main()
{
    #include "a0dynarray-use1.cci"
    std::sort( sorted.begin(), sorted.end() );
    #include "a0dynarray-use2.cci"
    #include "a0dynarray-use3a.cci"
    #include "a0dynarray-use4a.cci"
    return sum - 23;
}
