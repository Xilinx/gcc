// { dg-xfail-if "redefinition problems" { *-*-* } { "-fpph-map=pph.map" } }
// { dg-excess-errors "typeinfo redefinition problems.  May be merging issue for global binding" }

#include "x0keyed1.h"
#include "x0keyed2.h"

int keyed::key( int arg ) { return mix( field & arg ); }

int main()
{
    keyed variable;
    return variable.key( 3 );
}
