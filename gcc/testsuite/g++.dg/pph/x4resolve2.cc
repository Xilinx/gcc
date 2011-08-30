// pph asm xwant 12527

#include "x0resolve2.h"
#include "x0resolve1.h"

int caller()
{
    return overloader1() + overloader2();
}
