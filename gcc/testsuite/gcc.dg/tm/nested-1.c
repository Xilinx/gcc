/* { dg-do compile } */
/* { dg-options "-fgnu-tm" } */

extern int foo(int) __attribute__((transaction_safe));
void bar(void)
{
  __transaction {
    if (foo(1))
      __transaction {
        if (foo(2))
          __transaction {
            if (foo(3))
              __transaction {
                if (foo(4))
                  foo(5);
                else
                  __transaction_cancel;
              }
            else
              __transaction_cancel;
          }
        else
          __transaction_cancel;
      }
    else
      __transaction_cancel;
  }
}
