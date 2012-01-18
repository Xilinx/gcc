#include "x0namespace.h"
#include "x0namespace2.h"

namespace A {

int x = 3;
int x2 = 5;

C< int > z;
C2< int > z2;

} // namespace A

int y = 4;
int y2 = 6;

int D::method()
{ return y; }

int D2::method()
{ return y2; }

int main()
{
    A::z.method();
    A::z2.method();
}
