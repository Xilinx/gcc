#include "x0overload1.h"
#include "x0overload2.h"
#include "x1overload3.h"
// { dg-error "inconsistent overload resolution" "" { xfail *-*-* } }

int g()
{
   return f();
}
