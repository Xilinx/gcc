#ifndef X0RESOLVE2_H
#define X0RESOLVE2_H

int intfunc(long x);
int fltfunc(double x);

inline int overloader2()
{
    return intfunc(3) + fltfunc(2.0f);
}

#endif
