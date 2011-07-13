// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-do link }

#include "x0dynarray2.h"

typedef int integer;
typedef tst::dynarray< integer > integer_array;

int main()
{
    #include "a0dynarray-use1.cci"
    #include "a0dynarray-use2.cci"
    #include "a0dynarray-use4a.cci"
    return sum - 25;
}
