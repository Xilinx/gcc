// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }


#include "x0structover1.h"
#include "x0structover2.h"

int func2() {
    return func( (S*)0 );
}
