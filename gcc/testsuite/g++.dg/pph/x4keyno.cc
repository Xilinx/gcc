// { dg-xfail-if "redefinition problems" { *-*-* } { "-fpph-map=pph.map" } }
// { dg-excess-errors "The variable for the typeinfo name for 'keyno' is duplicated." }

#include "x0keyno1.h"
#include "x0keyno2.h"

int main()
{
    keyno variable;
    return variable.mix( 3 );
}
