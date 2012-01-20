#include "x1overset1.h"
#include "x1overset2.h"
#include "x1overset1.h"
#include "x1overset2.h"

int caller()
{
    derived1 v1;
    derived2 v2;
    int t = func(&v1) + func(&v2);
    t += overloader1() + overloader2();
    return t;
}
