/* { dg-do compile } */
/* { dg-options "-fgnu-tm" } */

void ts(void) __attribute__((tm_safe));
void tp(void) __attribute__((tm_pure));
void tc(void) __attribute__((tm_callable));
void ti(void) __attribute__((tm_irrevocable));
void tu(void);
int fc(int) __attribute__((const));

int __attribute__((tm_safe))
foo(void)
{
  int i;

  ts();
  tp();
  tc();			/* { dg-error "unsafe function call" } */
  ti();			/* { dg-error "unsafe function call" } */
  tu();			/* { dg-error "unsafe function call" } */

  asm("");		/* { dg-error "asm not allowed" } */
  asm("" : "=g"(i));	/* { dg-error "asm not allowed" } */

  i = fc(i);

  return i;
}
