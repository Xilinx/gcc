/* { dg-do compile } */
/* { dg-options "-fgnu-tm -fdump-ipa-tmipa" } */

/* Test that a direct call to __builtin__ITM_changeTransactionMode()
   sets the irrevocable bit.  */

int global;
int george;

foo()
{
	__tm_atomic {
		global++;
		__builtin__ITM_changeTransactionMode ();
		george++;
	}
}

/* { dg-final { scan-ipa-dump-times "GTMA_MAY_ENTER_IRREVOKABLE" 1 "tmipa" } } */
