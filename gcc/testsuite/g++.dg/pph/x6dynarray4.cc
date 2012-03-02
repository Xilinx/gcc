// {    xfail-if "DEPENDENT" { "*-*-*" } { "-fpph-map=pph.map" } }

#include "x6dynarray5.h" // { dg-bogus "cannot open PPH file x6dynarray5.pph" "" { xfail *-*-* } }

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
