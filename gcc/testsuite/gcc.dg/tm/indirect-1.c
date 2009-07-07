/* { dg-do compile } */
/* { dg-options "-fgnu-tm" } */

void foo(void (*fn)(void))
{
  __tm_atomic {
    fn();
  }
}
