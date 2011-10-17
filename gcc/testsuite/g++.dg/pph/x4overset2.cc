// pph asm xwant 57625
// This test produces overload differences because the declaration and
// call orders are different between pph and textual parsing.

#include "x1overset2.h"
#include "x1overset1.h"

int caller()
{
    derived1 v1;
    derived2 v2;
    int t = func(&v1) + func(&v2);
    t += overloader1() + overloader2();
    return t;
}
