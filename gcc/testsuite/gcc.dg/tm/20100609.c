/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O" } */

extern void funcNoReturn() __attribute__ ((__noreturn__));

int later;

void MyFunc()
{
  __transaction [[relaxed]] {
        funcNoReturn();
        later=8;
  }
}
