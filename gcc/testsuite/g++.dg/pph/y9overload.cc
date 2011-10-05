#include "x0overload1.h"
#include "x0overload2.h"
#include "x1overload3.h"
// When this test case is compiled in PPH mode, we should be generating
// a diagnostic because overload resolution is different than in
// the non PPH compile.  The call to o() from x1overload3.h should
// resolve differently when x0overload1.h and x0overload2.h are PPH
// images.
// { dg-error "inconsistent overload resolution" "" { xfail *-*-* } }

int g()
{
   return f();
}
