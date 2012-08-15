#include <stdio.h>

int main(int argc, char **argv)
{
  int array[10][15], ii = 0, jj = 0,x = 0, z= 1 , y = 10 ;
  int array_2[10][15];
 
  if (argc != 3)
    {
      fprintf(stderr, "Usage: %s 10 15\n", argv[0]);
      return -1;
    }
  printf("==============================================\n"); 
  for (ii = 0; ii < 10; ii++) {
    for (jj = 0; jj< 15; jj++) {
      array[ii][jj] = ii+jj;
      array_2[ii][jj] = 0;
    }
  }
  array_2[0:5:2][0:5:3] = array[0:5:2][0:5:3] + 1 + 5 + array[0][5] + x;

  for (ii = 0; ii < 10; ii++)
    {
      for (jj = 0; jj < 15; jj++)
	{
	  printf("%2d ", array_2[ii][jj]);
	}
      printf("\n");
    }


  printf("==============================================\n"); 
  for (ii = 0; ii < 10; ii++) {
    for (jj = 0; jj< 15; jj++) {
      array[ii][jj] = ii+jj;
      array_2[ii][jj] = 0;
    }
  }
  x = atoi(argv[1]);
  y = atoi(argv[2]);
  array_2[0:x:1][0:y:1] = array[0:x:1][0:y:1] + x + y + array[0:x:1][0:y:1];

  for (ii = 0; ii < 10; ii++)
    {
      for (jj = 0; jj < 15; jj++)
	{
	  printf("%2d ", array_2[ii][jj]);
	}
      printf("\n");
    }

  printf("==============================================\n"); 
  for (ii = 0; ii < 10; ii++) {
    for (jj = 0; jj< 15; jj++) {
      array[ii][jj] = ii+jj;
      array_2[ii][jj] = 0;
    }
  }
  x = atoi(argv[1]);
  y = atoi(argv[2]);
  z = (20- atoi (argv[1]))/atoi(argv[1]);
  /* (20-10)/10 evaluates to 1 all the time :-). */
  array_2[0:x:z][0:y:z] = array[0:x:z][0:y:z] + array[0:x:z][0:y:z] + y + z;
  printf("x = %2d\ty = %2d\tz = %2d\n", x, y, z);
  for (ii = 0; ii < 10; ii++)
    {
      for (jj = 0; jj < 15; jj++)
	{
	  printf("%2d ", array_2[ii][jj]);
	}
      printf("\n");
    }

#if 1
  printf("==============================================\n"); 
  for (ii = 0; ii < 10; ii++) {
    for (jj = 0; jj< 15; jj++) {
      array[ii][jj] = ii+jj;
      array_2[ii][jj] = 0;
    }
  }
  x = argc-3;
  y = 20-atoi(argv[1]);
  z = (20- atoi (argv[1]))/atoi(argv[1]);
  /* (20-10)/10 evaluates to 1 all the time :-). */
  array_2[(argc-3):(20-atoi(argv[1])):(20-atoi(argv[1]))/atoi(argv[1])][(argc-3):(30-atoi(argv[2])): ((30-atoi(argv[2]))/atoi(argv[2]))] = array[(argc-3):20-atoi(argv[1]):(20-atoi(argv[1]))/atoi(argv[1])][(argc-3):(30-atoi(argv[2])): (30-atoi(argv[2]))/atoi(argv[2])] + array[(argc-3):20-atoi(argv[1]):(20-atoi(argv[1]))/atoi(argv[1])][(argc-3):(30-atoi(argv[2])): (30-atoi(argv[2]))/atoi(argv[2])] * array[(argc-3):20-atoi(argv[1]):(20-atoi(argv[1]))/atoi(argv[1])][(argc-3):(30-atoi(argv[2])): (30-atoi(argv[2]))/atoi(argv[2])];
  printf("x = %2d\ty = %2d\tz = %2d\n", x, y, z);
  for (ii = 0; ii < 10; ii++)
    {
      for (jj = 0; jj < 15; jj++)
	{
	  printf("%4d ", array_2[ii][jj]);
	}
      printf("\n");
    }
#endif
  return 0;
}
