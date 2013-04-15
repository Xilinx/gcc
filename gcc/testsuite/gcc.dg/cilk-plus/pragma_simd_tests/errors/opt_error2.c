/* { dg-do compile } */
/* { dg-options "-O3 -Wunknown-pragmas" } */

int main(void)
{ 
  int ii,  A[1000];
#pragma simd  /* { dg-warning "ignoring" } */
  for (ii = 0; ii < 1000; ii++)
    {
      A[ii] = ii;
    }
  return 0;
}
