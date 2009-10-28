/* { dg-do compile } */
/* { dg-options "-fgnu-tm" } */

void mco(void) __attribute__((transaction_may_cancel_outer));

void
f(void)
{
  mco();			/* { dg-error "" } */
  __transaction {
    mco();			/* { dg-error "" } */
  }
  __transaction [[relaxed]] {
    mco();			/* { dg-error "" } */
  }
  __transaction [[outer]] {
    mco();
  }
  __transaction [[outer]] {
    __transaction {
      __transaction {
        __transaction {
          mco();
        }
      }
    }
  }
}

void __attribute__((transaction_may_cancel_outer))
g(void)
{
  mco();
  __transaction {
    __transaction {
      __transaction {
        __transaction {
          mco();
        }
      }
    }
  }
}
