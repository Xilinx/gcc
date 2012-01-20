// { dg-bogus "a0dynarray-dfn1b.hi:22:5: error: no suitable 'operator delete" "" { xfail *-*-* } 0 }
// { dg-bogus "a0dynarray-dfn2b.hi:13:9: error: no suitable 'operator delete" "" { xfail *-*-* } 0 }
// { dg-bogus "a0dynarray-dcl3.hi:11:60: error: call of overloaded 'operator new" "" { xfail *-*-* } 0 }

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
