#ifndef X0RESOLVE1_H
#define X0RESOLVE1_H

int intfunc(int x);
int fltfunc(float x);

inline int overloader1()
{
    return intfunc(3l) + fltfunc(2.0);
}

#endif
