// pph asm xdiff 52472
//FIXME Emitting a second copy of the non-inline function f.
//FIXME The linker should choke over this one.

#include "e0noninline1.h"
#include "e0noninline2.h"

int main()
{
  return g() + h();
}
