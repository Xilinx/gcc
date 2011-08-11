#ifndef A0TMPLFUNCINLN_G_H
#define A0TMPLFUNCINLN_G_H

template< typename T >
inline T function1( T t )
{
    return t * 2;
}

template<> double function1(double d)
{
    return d * 3.0;
}

inline char instantiator1(char c)
{
    return function1(c);
}

#endif
