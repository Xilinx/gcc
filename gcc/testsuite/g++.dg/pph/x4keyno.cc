// { dg-xfail-if "BOGUS MERGE AUXVAR" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x4keyno.cc:12:1: error: redefinition of 'const char _ZTS5keyno" "" { xfail *-*-* } 0 }
// The variable for the typeinfo name for 'keyno' is duplicated.

#include "x0keyno1.h"
#include "x0keyno2.h"

int main()
{
    keyno variable;
    return variable.mix( 3 );
}
