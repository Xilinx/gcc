#ifndef X1TMPLFUNC_H
#define X1TMPLFUNC_H
typedef int type;
extern type val;

template< typename T >
T identity(T arg)
{ return arg + val; }
#endif
