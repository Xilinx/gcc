namespace A {
int x = 3;
struct B;
struct C {
    B* b;
    int method();
    int another()
    { return 1; }
};
int C::method()
{ return 1; }
} // namespace A
struct D : A::C {
    int method();
    int another()
    { return 1; }
};
int D::method()
{ return 1; }
int main()
{ }
