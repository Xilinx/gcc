// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "x4keyno.cc:11:1: error: redefinition of 'const char _ZTS5keyno" "" { xfail *-*-* } 0 }

#include "x0keyno1.h"
#include "x0keyno2.h"

int main()
{
    keyno variable;
    return variable.mix( 3 );
}
