// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-excess-errors "Embedded text merging problems" }
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
