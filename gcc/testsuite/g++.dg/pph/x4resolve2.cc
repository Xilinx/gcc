// pph asm xwant 37643
// This test produces overload differences because the declaration and
// call orders are different between pph and textual parsing.

#include "x0resolve2.h"
#include "x0resolve1.h"

int caller()
{
    int t = intfunc(3l) + fltfunc(2.0);
    t += intfunc(3) + fltfunc(2.0f);
    t += overloader1() + overloader2();
    return t;
}
