#include <stdio.h>
#include <stdlib.h>

main() 
{
  int i, N, *p;
  char buf[3];

  printf("Input a number less than 100: ");
  flockfile(stdin);
  fgets(buf, 3, stdin);
  N = atoi(buf);
  printf("\nExecuting %d iterations\n", N);
  for(i = 0; i<N; i++) 
    {
      p = malloc(sizeof(int));
      if(!p) 
	{
	  printf("failed to malloc p\n");
	  funlockfile(stdin);
	  return -1;
	}
      if(!*p)
	{
	  fprintf(stdout, "%d\n", i);
	}
    }

  funlockfile(stdin);
  return 0;
}  /* { dg-warning "unfreed" } */
