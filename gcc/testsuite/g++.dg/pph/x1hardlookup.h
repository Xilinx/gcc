#ifndef X1HARDLOOKUP_H
#define X1HARDLOOKUP_H
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
int operator + (int i, D d);
#endif
