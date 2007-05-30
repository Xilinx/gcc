/* { dg-do compile } */ 
/* { dg-options "-O2 -ftree-loop-distribution -fdump-tree-ldist-all" } */

int loop1 (int k)
{
  unsigned int i;
  unsigned int j;
  int a[100][100], b[100][100], c[100][100], d[100][100];

  a[0][0] = k;
  for (i = 1; i < 100; i ++)
    {
      for (j = 0; j < 100; j++) 
	{		
	  a[i][j] = k * i;
	  b[i][j] = a[i-1][j] + k;
	}
      for (j = 0; j < 100; j++)
	{
	  c[i][j] = b[i][j] + a[i-1][j];
	  d[i][j] = c[i-1][j] + k + i;
	}
    }
				
  return d[100-1][0];
}

/* { dg-final { scan-tree-dump-times "distributed" 1 "ldist" { xfail *-*-* } } } */
/* { dg-final { cleanup-tree-dump "ldist" } } */
