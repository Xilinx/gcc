/* { dg-do compile } */
/* { dg-options "-fgnu-tm" } */

void foobar(void)
{
    __transaction {
       foobar();
    }
}

void doit(void) __attribute__((transaction_safe));

__attribute__((transaction_callable))
void callable(void)
{
  __transaction {
    doit();
  }
}
