#ifndef A0TMPLFUNCNINL_S_H
#define A0TMPLFUNCNINL_S_H

template int function2(int);

template<> double function2(double d)
{
    return d * 4.0;
}

#endif
