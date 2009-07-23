/* { dg-do compile } */

void f(void)
{
  __tm_atomic { /* { dg-error "__tm_atomic. without" } */
    __tm_abort; /* { dg-error "__tm_abort. without" } */
  }
}
