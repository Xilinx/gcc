// { dg-xfail-if "BOGUS TYPE MERGE" { "*-*-*" } { "-fpph-map=pph.map" } }
// This test fails because even if we merge type decls,
// we are not merging the corresponding types.

#include "x0structover1.h"
#include "x0structover2.h"

int func2() {
    return func( (S*)0 );
}
