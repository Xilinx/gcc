// pph asm xdiff 10014
//FIXME Emitting a second copy of the explicit instantiation of 'function'.
//FIXME With comdat, the linker may paper over the differences.

#include "z0expinstinl1.h"
#include "z0expinstinl2.h"

int main()
{
  int var = 1;
  return function(var);
}
