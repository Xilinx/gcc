// {    xfail-if "DEPENDENT" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "ext/atomicity.h:48:61: error: void value not ignored as it ought to be" "" { xfail *-*-* } 0 }

#include "x0dynarray4.h"
#include "x6dynarray5.h" // { dg-bogus "warning: cannot open PPH file x6dynarray5.pph" "" { xfail *-*-* } }

#include <algorithm>
#include <iostream>

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
    #include "a0dynarray-use3a.cci"
    #include "a0dynarray-use4a.cci"
    return sum - 23;
}
