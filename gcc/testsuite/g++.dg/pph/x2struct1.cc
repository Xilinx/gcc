#include "x1struct1.h"

int D::method()
{ static int x = 2;
  return fld + mbr; }

int D::mbr = 4;
D var1;
D var2 = var1;
