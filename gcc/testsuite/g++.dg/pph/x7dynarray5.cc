// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "wchar.h:1:0: error: PPH file stdio.pph fails macro validation, _WCHAR_H" "" { xfail *-*-* } 0 }
// { dg-bogus "unistd.h:1144:34: error: declaration of .* ctermid.* has a different exception specifier" "" { xfail *-*-* } 0 }
// { dg-bogus "stdio.h:858:14: error: from previous declaration .* ctermid.*" "" { xfail *-*-* } 0 }

#include "x0dynarray4.h"
#include "x6dynarray5.h"

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
