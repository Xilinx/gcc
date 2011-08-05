#include "c0rawstruct1.h"
#include "c0rawstruct2.h"

struct S s = { 5, 6 };;

int main()
{
  return g() + h() + s.a + s.b;
}
