// pph asm xdiff 46318
// Looks like destructors botched.

#include "x0keyno1.h"

int main()
{
    keyed variable;
    return variable.mix( 3 );
}
