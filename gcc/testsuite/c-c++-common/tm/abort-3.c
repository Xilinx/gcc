/* { dg-do compile } */

void f(void)
{
  __transaction {	   /* { dg-error "__transaction. without trans" } */
    __transaction_cancel;  /* { dg-error "_cancel. without trans" } */
  }
}
