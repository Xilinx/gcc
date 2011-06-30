/* { dg-xfail-run-if "unsupported" { *-*-* } } */
#include <libitm.h>

char *pp;

int main()
{
  __transaction {
    _ITM_dropReferences (pp, 555);
  }
  return 0;
}
