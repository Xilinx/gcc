// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "cstdlib:42:0: error: PPH file stdlib.pph fails macro validation, __STDDEF_H__" "" { xfail *-*-* } 0 }

#include "x1dynarray5.h"

#include <algorithm>

#include "a1integer.h"
typedef tst::dynarray< integer > integer_array;

int main()
{
    #include "a1dynarray-use1.cci"
    std::sort( sorted.begin(), sorted.end() );
    #include "a1dynarray-use2.cci"
    #include "a1dynarray-use3a.cci"
    #include "a1dynarray-use4a.cci"
    return sum - 23;
}
