/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O" } */

int global;
int george;

extern crap() __attribute__((tm_irrevocable));

foo()
{
	__tm_atomic {
		global++;
		crap();
		george++;
	}
}
