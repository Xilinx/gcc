/* { dg-do run } */
/* { dg-options "-O3 -fcilkplus" } */


/* This is a simple vectorization, it checks if check_off_reduction_var works 
   and it also checks if it can vectorize this loop in func correctly. */
#define N 1000

int func (int *p, int *q) {
    int ii;
    int x = 0;
#pragma simd reduction (+:x)
    for (ii = 0; ii < N; ii++) { 
	x += (q[ii] + p[ii]);
    }
    return x; 

}

int main (int argc, char **argv)
{
  int ii = 0, x;
  int Array[N], Array2[N];

  for (ii = 0; ii < N; ii++)
    {
      Array[ii] = 5 + argc;
      Array2[ii] = argc;
    }
  x = func (Array, Array2);

  if (x != N * 7)
    return 1;
  return 0;
}

