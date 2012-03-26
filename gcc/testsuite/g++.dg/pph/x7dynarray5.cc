// { dg-xfail-if "DEPENDENT" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "streambuf_iterator.h:372:58: internal compiler error: canonical types differ for identical types.*" "" { xfail *-*-* } 0 }
// { dg-bogus "ext/atomicity.h:48:61: error: void value not ignored as it ought to be" "" { xfail *-*-* } 0 }
// { dg-bogus "basic_string.h:111:63: error: no class template named .rebind. in .class std::allocator<char>." "" { xfail *-*-* } 0 }
// { dg-bogus "basic_string.h:111:63: error: no class template named .rebind. in .class std::allocator<wchar_t>." "" { xfail *-*-* } 0 }
// { dg-bogus "basic_string.h:124:68: error: no class template named .rebind. in .class std::allocator<char>." "" { xfail *-*-* } 0 }
// { dg-bogus "basic_string.h:124:68: error: no class template named .rebind. in .class std::allocator<wchar_t>." "" { xfail *-*-* } 0 }
// { dg-bogus "basic_string.h:126:61: error: no class template named .rebind. in .class std::allocator<char>." "" { xfail *-*-* } 0 }
// { dg-bogus "basic_string.h:126:61: error: no class template named .rebind. in .class std::allocator<wchar_t>." "" { xfail *-*-* } 0 }
// { dg-bogus "basic_string.h:127:53: error: no class template named .rebind. in .class std::allocator<char>." "" { xfail *-*-* } 0 }
// { dg-bogus "basic_string.h:127:53: error: no class template named .rebind. in .class std::allocator<wchar_t>." "" { xfail *-*-* } 0 }
// { dg-bogus "basic_string.h:128:52: error: no class template named .rebind. in .class std::allocator<char>." "" { xfail *-*-* } 0 }
// { dg-bogus "basic_string.h:128:52: error: no class template named .rebind. in .class std::allocator<wchar_t>." "" { xfail *-*-* } 0 }
// { dg-bogus "basic_string.h:155:56: error: no class template named .rebind. in .class std::allocator<char>." "" { xfail *-*-* } 0 }
// { dg-bogus "basic_string.h:155:56: error: no class template named .rebind. in .class std::allocator<wchar_t>." "" { xfail *-*-* } 0 }
// { dg-bogus "locale_classes.tcc:47:46: error: call of overloaded .operator new.sizetype.. is ambiguous" "" { xfail *-*-* } 0 }
// { dg-bogus "locale_classes.tcc:65:43: error: call of overloaded .operator new.sizetype.. is ambiguous" "" { xfail *-*-* } 0 }

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
