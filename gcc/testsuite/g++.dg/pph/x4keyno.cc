// pph asm xdiff 17472
// Looks like destructors botched.

#include "x0keyno1.h"
#include "x0keyno2.h"

int main()
{
    keyed variable;
    return variable.mix( 3 );
}
