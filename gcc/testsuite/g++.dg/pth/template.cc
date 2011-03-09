namespace A {
int x = 3;
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
int y = 4;
struct D : A::C< int > {
    int method();
    int another()
    { return *b; }
};
int D::method()
{ return y; }
int main()
{ }
