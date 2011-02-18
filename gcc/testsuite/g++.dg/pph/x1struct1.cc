#include "x1struct1.h"
type D::method()
{ static int x = 2;
  return fld + mbr; }
type D::mbr = 4;
typedef D D2;
D2 var1;
D2 var2 = var1;
