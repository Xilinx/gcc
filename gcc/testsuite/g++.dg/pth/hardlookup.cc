struct V { int a; static int b;};
namespace N {
   int V;
   struct C { };
   int operator + (int i, C c);
   C O;
   int I(int arg)
   { return arg + V; }
   struct V w;
   int x = V::b;
}

int V;
struct D { };
D P;
int operator + (int i, D d);
int I(int arg)
{ return arg + V; }

int F() {
   return I(N::V + N::O);
}

int G() {
   return I(::V + P);
}

struct V w;
int x = V::b;
