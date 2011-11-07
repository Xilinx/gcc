// { dg-bogus "x4namespace.cc:11:1: error: 'C' does not name a type" "" { xfail *-*-* } 0 }
// { dg-bogus "x4namespace.cc:27:5: error: 'z' is not a member of 'A'" "" { xfail *-*-* } 0 }

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
