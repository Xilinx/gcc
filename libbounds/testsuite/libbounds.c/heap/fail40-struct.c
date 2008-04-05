typedef struct {
  int i;
} sTest;

int main()
{
    sTest *p = (sTest *) malloc( sizeof(sTest) );

    p++;
    p->i = 7;

    return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
