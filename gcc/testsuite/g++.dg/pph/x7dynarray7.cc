// { dg-xfail-if "BOGUS INTRINSIC RETURN" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "ext/atomicity.h:48:61: error: void value not ignored as it ought to be" "" { xfail *-*-* } 0 }

#include <algorithm>
#include <iostream>

#include "x5dynarray7.h"

#include "a0integer.h"
typedef tst::dynarray< integer > integer_array;

#include "a0dynarray-hlp1.cci"

int main()
{
    #include "a0dynarray-use1.cci"
    dump( source );
    dump( sorted );
    std::sort( sorted.begin(), sorted.end() );
    dump( sorted );
    #include "a0dynarray-use2.cci"
    dump( target );
    #include "a0dynarray-use3b.cci"
    #include "a0dynarray-use4b.cci"
    return sum - 23;
}
