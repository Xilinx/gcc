/* { dg-do compile } */ 
/* { dg-options "-O2 -fgraphite -fdump-tree-graphite-all" } */

void bar (void);

int toto()
{
  /* Scop 1. */
  int i, j, k;
  int a[100][100];
  int b[100];
  /* End scop 1. */

  for (i = 1; i < 100; i++)
    {
      /* Scop 2. */
      for (j = 1; j < 100; j++)
	a[j][i] = a[j+1][i-1] + 2;

      b[i] = b[i-1] + 2;
      /* End scop 2. */

      bar ();

      /* Scop 3. */
      for (j = 1; j < 100; j++)
	a[j][i] = a[j+1][i-1] + 2;

      b[i] = a[i-1][i] + 2;

      for (j = 1; j < 100; j++)
	a[j][i] = a[j+1][i-1] + 2;
      /* End scop 3. */
    }

  /* Scop 4. */
  return a[3][5] + b[1];
  /* End scop 4. */
}

/* { dg-final { scan-tree-dump-times "number of SCoPs: 4" 1 "graphite"} } */ 
/* { dg-final { cleanup-tree-dump "graphite" } } */
