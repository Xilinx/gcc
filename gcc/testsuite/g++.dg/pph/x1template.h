#ifndef X1TEMPLATE_H
#define X1TEMPLATE_H
namespace A {
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
} // namespace A
struct D : A::C< int > {
    int method();
    int another()
    { return *b; }
};
#endif
