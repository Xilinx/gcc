/* { dg-do compile } */ 
/* { dg-options "-O2 -fgraphite -fdump-tree-graphite-all" } */
#define FLOAT float

int foo (void);

/* Multiply two n x n matrices A and B and store the result in C.  */

void matmult (FLOAT **A, FLOAT **B, FLOAT **C, int n)
{
  int i,j,k;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      for (k = 0; k < n; k++)
        A[i][j] += B[i][k] * C[k][j];
}

/* { dg-final { scan-tree-dump-times "number of SCoPs: 3" 1 "graphite"} } */ 
/* { dg-final { cleanup-tree-dump "graphite" } } */
