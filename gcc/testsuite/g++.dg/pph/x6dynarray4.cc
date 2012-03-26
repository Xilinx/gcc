// { xfail-if "DEPENDENT" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "a0dynarray-dcl3.hi:11:60: error: call of overloaded .operator new ...sizetype.. is ambiguous" "" { xfail *-*-* } 0 }
// { dg-bogus "stl_algobase.h:625:18: error: .__value. is not a member of .std::__is_move_iterator" "" { xfail *-*-* } 0 }
// { dg-bogus "stl_algobase.h:276:12: error: .__value. is not a member of .std::__is_move_iterator" "" { xfail *-*-* } 0 }
// { dg-bogus "stl_algobase.h:625:18: error: no matching function for call to .__miter_base" "" { xfail *-*-* } 0 }
// { dg-bogus "stl_algobase.h:282:5: error: no type named .iterator_type. in .struct std::_Miter_base." "" { xfail *-*-* } 0 }
// { dg-bogus "stl_algobase.h:139:7: error: .__value. is not a member of .std::__are_same" "" { xfail *-*-* } 0 }

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
