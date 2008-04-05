typedef struct {
  short a[3];
} sTest;

int main()
{
    sTest *p = (sTest *) malloc( sizeof(sTest) );

    p->a[3] = 4;

    return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
