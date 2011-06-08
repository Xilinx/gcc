// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "c1typerefs.h:11:18: error: cannot convert 'const std::type_info.' to 'const std::type_info.' in initialization" "" { xfail *-*-* } 0 }

#include "x1typerefs.h"

int derived::method() {
    return field;
}

vderived variable;
