#ifndef A0TMPLFUNCNINL_G_H
#define A0TMPLFUNCNINL_G_H

template< typename T >
T function2( T t )
{
    return t * 3;
}

inline char instantiator2(char c)
{
    return function2(c);
}

#endif
