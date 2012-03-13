#include "x0keyed1.h"
#include "x0keyed2.h"

int keyed::key( int arg ) { return mix( field & arg ); }

int main()
{
    keyed variable;
    return variable.key( 3 );
}
