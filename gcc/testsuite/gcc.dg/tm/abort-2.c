/* { dg-do compile } */
/* { dg-options "-fgnu-tm" } */

int g;
void f(void)
{
  __tm_atomic {
    if (g == 0)
      __tm_abort;
  }
}
