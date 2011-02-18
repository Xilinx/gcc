#include "x2overload1.h"
#include "x2overload2.h"
#include "x2overload3.h"
// { dg-error "inconsistent overload resolution" "" { xfail *-*-* } }

int g()
{
   return f();
}
