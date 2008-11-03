/* { dg-do compile } */

void f(void)
{
  __tm_abort;		/* { dg-error "not within" } */
}

void g(void)
{
  __tm_retry;		/* { dg-error "not within" } */
}
