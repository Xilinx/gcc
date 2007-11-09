/* Invalid __ea declarations.  */
/* { dg-do compile } */

__ea int i0;		 /* { dg-error "'__ea' variable 'i0' must be extern" } */
static int __ea i2;	 /* { dg-error "'__ea' combined with 'static' qualifier for 'i2'" } */
extern int __ea __ea i4; /* { dg-error "duplicate '__ea'" } */
extern __ea void f1 ();	 /* { dg-error "'__ea' specified for function 'f1'" } */

void func ()
{
  register __ea int local1; /* { dg-error "'__ea' combined with 'register' qualifier for 'local1'" } */
  auto __ea int local2;     /* { dg-error "'__ea' combined with 'auto' qualifier for 'local2'" } */
  __ea int local3;	    /* { dg-error "'__ea' specified for auto variable 'local3'" } */
  static __ea int local4;   /* { dg-error "'__ea' combined with 'static' qualifier for 'local4'" } */
}

void func2 (__ea int x)	    /* { dg-error "'__ea' specified for parameter 'x'" } */
{ }

struct st {
  __ea int x;		    /* { dg-error "'__ea' specified for structure field 'x'" } */
  __ea int *p;		    /* { dg-error "'__ea' specified for structure field 'p'" } */
} s;

__ea int func3 (int x) {    /* { dg-error "'__ea' specified for function 'func3'" } */
  return x;
}
