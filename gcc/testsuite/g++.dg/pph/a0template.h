#ifndef A0TEMPLATE_H
#define A0TEMPLATE_H
extern int x;
struct B;
template< typename T >
struct C {
    T* b;
    int method();
    int another()
    { return *b; }
};
template< typename T >
int C< T >::method()
{ return x; }
struct D : C< int > {
    int method();
    int another()
    { return *b; }
};
#endif
