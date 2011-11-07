#ifndef X0NAMESPACE2_H
#define X0NAMESPACE2_H
namespace A {
extern int x2;
struct B2;
template< typename T >
struct C2 {
    T* b;
    int method();
    int another()
    { return *b; }
};
template< typename T >
int C2< T >::method()
{ return x2; }
} // namespace A
struct D2 : A::C2< int > {
    int method();
    int another()
    { return *b; }
};
#endif
