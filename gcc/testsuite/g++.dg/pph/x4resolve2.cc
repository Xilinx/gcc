// pph asm xwant 37643

#include "x0resolve2.h"
#include "x0resolve1.h"

int caller()
{
    int t = intfunc(3l) + fltfunc(2.0);
    t += intfunc(3) + fltfunc(2.0f);
    t += overloader1() + overloader2();
    return t;
}
