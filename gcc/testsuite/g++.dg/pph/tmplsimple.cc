template< typename T >
struct C {
    int* b;
    int method();
    int another()
    { return 1; }
};
template< typename T >
int C< T >::method()
{ return 1; }
C<int> v;
