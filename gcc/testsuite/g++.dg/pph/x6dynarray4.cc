// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "cstdlib:42:0: error: PPH file stdlib.pph fails macro validation, __STDDEF_H__" "" { xfail *-*-* } 0 }

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
