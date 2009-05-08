void abort (void);

void parloop (int N)
{
  int i, j;
  int Z[3000][3000];

  for (i = 0; i <= N; i++)
    for (j = 0; j <= N; j++)
      Z[i][j] = Z[j+N][i+N+1];

  for (i = 0; i <= N; i++)
    for (j = 0; j <= N; j++)
      if (Z[i][j] != Z[j+N][i+N+1])
	abort();
}

int main(void)
{
  parloop(1000);

  return 0;
}

/* Check that parallel code generation part make the right answer.  */
/* { dg-final { scan-tree-dump-times "loopfn" 5 "final_cleanup" } } */
/* { dg-final { cleanup-tree-dump "parloops" } } */
/* { dg-final { cleanup-tree-dump "final_cleanup" } } */
