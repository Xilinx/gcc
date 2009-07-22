/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O" } */

int global;
int george;

extern crap() __attribute__((tm_irrevokable));

foo()
{
	__tm_atomic {
		global++;
		crap();
		george++;
	}
}
