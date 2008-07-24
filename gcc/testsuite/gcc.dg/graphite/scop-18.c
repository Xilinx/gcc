/* { dg-do compile } */ 
/* { dg-options "-O2 -floop-block -fdump-tree-graphite-all" } */

#define N 24
#define M 1000

void test (float **A, float **B, float **C)
{
  int i, j, k;

  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      for (k = 0; k < N; k++)
        A[i][j] += B[i][k] * C[k][j];
  

  for (i = 0; i < M; i++)
    for (j = 0; j < M; j++)
      for (k = 0; k < M; k++)
        A[i][j] += B[i][k] * C[k][j];
}

/* Strip Mining is not profitable for loops 0, 1 and 2. */

/* { dg-final { scan-tree-dump-times "Strip Mining is not profitable" 3 "graphite" } } */
/* { dg-final { cleanup-tree-dump "graphite" } } */
