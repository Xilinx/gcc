/* { dg-do compile } */
/* { dg-options "-fcilkplus -O2" } */

int main(void)
{
 int ii,  A[1000];
#pragma simd  /* { dg-warning "pragma simd is useless without" } */
  for (ii = 0; ii < 1000; ii++)
    {
      A[ii] = ii;
    }
  return 0;
}
