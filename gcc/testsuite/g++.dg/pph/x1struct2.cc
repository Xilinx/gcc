#include "x1struct2.h"
type D::method()
{ static int x = 2;
  return fld + mbr + gbl; }
type D::mbr = 4;
typedef D D2;
D2 var1;
D2 var2 = var1;
