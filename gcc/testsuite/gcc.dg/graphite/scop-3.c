/* { dg-do compile } */ 
/* { dg-options "-O2 -fgraphite -fdump-tree-graphite-all" } */

int toto()
{
  /* Scop 1. */
  /* End scop 1. */
  int i, j, k;
  int a[100][100];
  int b[100];

  /* Scop 2. */
  for (i = 1; i < 100; i++)
    {
      for (j = 1; j < 80; j++)
	a[j][i] = a[j+1][2*i-1*j] + 12;

      b[i] = b[i-1] + 10;

      for (j = 1; j < 60; j++)
	a[j][i] = a[j+1][i-1] + 8;

      if (i == 23)
	b[i] = a[i-1][i] + 6;

      for (j = 1; j < 40; j++)
	a[j][i] = a[j+1][i-1] + 4;
    }
  /* End scop 2. */

  /* Scop 3. */
  return a[3][5] + b[1];
  /* End scop 3. */
}

/* { dg-final { scan-tree-dump-times "number of SCoPs: 3" 1 "graphite"} } */ 
/* { dg- final { cleanup-tree-dump "graphite" } } */
