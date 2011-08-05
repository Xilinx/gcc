// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "a0keyed.h:14:1: error: redefinition of .const char _ZTS5keyed []." "" { xfail *-*-* } 0 }
// { dg-bogus "a0keyed.h:14:1: error: .const char _ZTS5keyed .7.. previously defined here" "" { xfail *-*-* } 0 }

#include "x0keyed1.h"
#include "x0keyed2.h"

int keyed::key( int arg ) { return mix( field & arg ); }

int main()
{
    keyed variable;
    return variable.key( 3 );
}
