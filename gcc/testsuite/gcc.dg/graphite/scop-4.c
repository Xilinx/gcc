/* { dg-do compile } */ 
/* { dg-options "-O2 -fgraphite -fdump-tree-graphite-all" } */

void bar ();

int toto()
{
  /* Scop 1. */
  /* End scop 1. */
  int i, j, k;
  int a[100][100];
  int b[100];

  /* Scop 4. */
  for (i = 1; i < 100; i++)
  /* End scop 4. */
    {
      /* Scop 2. */
      for (j = 1; j < 80; j++)
	a[j][i] = a[j+1][2*i-1*j] + 12;

      b[i] = b[i-1] + 10;

      for (j = 1; j < 60; j++)
	a[j][i] = a[j+1][i-1] + 8;
      /* End scop 2. */

      bar ();

      /* Scop 3. */
      if (i == 23)
	b[i] = a[i-1][i] + 6;
      /* End scop 3. */
    }

  /* Scop 5. */
  return a[3][5] + b[1];
  /* End scop 5. */
}

/* { dg-final { scan-tree-dump-times "number of SCoPs: 5" 1 "graphite"} } */ 
/* { dg-final { cleanup-tree-dump "graphite" } } */
