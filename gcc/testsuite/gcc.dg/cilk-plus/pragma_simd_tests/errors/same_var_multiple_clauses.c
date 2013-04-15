/* { dg-do compile } */
/* { dg-options "-O3 -fcilkplus" } */

#define N 1000

int A[N], B[N], C[N];
int main (void)
{
  int ii = 0;

#pragma simd private (B) linear(B:1) /* { dg-error "listed in both linear and private pragma simd " } */
  for (ii = 0; ii < N; ii++)
    {
      A[ii] = B[ii] + C[ii];
    }

#pragma simd private (B, C) linear(B:1) /* { dg-error "listed in both linear and private pragma simd " } */
  for (ii = 0; ii < N; ii++)
    {
      A[ii] = B[ii] + C[ii];
    }

#pragma simd private (B) linear(C:2, B:1) /* { dg-error "listed in both linear and private pragma simd " } */
  for (ii = 0; ii < N; ii++)
    {
      A[ii] = B[ii] + C[ii];
    }

#pragma simd reduction (+:B) linear(B:1) /* { dg-error "listed in both reduction and linear pragma simd" } */
  for (ii = 0; ii < N; ii++)
    {
      A[ii] = B[ii] + C[ii];
    }

#pragma simd reduction (+:B) linear(B) /* { dg-error "listed in both reduction and linear pragma simd" } */
  for (ii = 0; ii < N; ii++)
    {
      A[ii] = B[ii] + C[ii];
    }

#pragma simd private (B) reduction (+:A) reduction (+:B) /* { dg-error "listed in both reduction and private pragma simd" } */
  for (ii = 0; ii < N; ii++)
    {
      A[ii] = B[ii] + C[ii];
    }

  return 0;
}
