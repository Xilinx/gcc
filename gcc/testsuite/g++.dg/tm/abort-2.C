// { dg-do compile }
// { dg-options "-fgnu-tm" }

int g;
void f(void)
{
  __transaction {
    if (g == 0)
      __transaction_cancel;
  }
}
