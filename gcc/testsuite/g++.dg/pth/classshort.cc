typedef int type;
type gbl = 1;
struct B {
    type fld;
};
struct D : B {
    type method();
    type another()
    { return fld + mbr + gbl; }
    type fld;
    static type mbr;
};
type D::method()
{ static int x = 2;
  return fld + mbr + gbl; }
type D::mbr = 4;
typedef D D2;
D2 var1;
D2 var2 = var1;
