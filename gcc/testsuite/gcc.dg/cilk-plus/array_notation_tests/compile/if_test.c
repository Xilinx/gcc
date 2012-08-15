#include <stdio.h>

int main (int argc, char **argv)
{
  int x = 3, y, z, array[10], array2[10], TwodArray[10][10], jj,kk,ll ;
  int FourDArray[10][10][10][10];
  int ii = 0; 

  if (argc != 3)
    {
      fprintf(stderr, "Usage: %s 10 15\n", argv[0]);
      return;
    }

  for (ii = 0; ii < 10; ii++)
    {
      array[ii] = argc%3;
      array2[ii]= 10;
    }
  
  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array[ii]);
  printf("\n");

  if (!array[:])
    array2[:] = 5;
  else
    array2[:] = 10;

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");

  if (!(array[0:10:1] + array[0:10:1]))
    array2[:] = 5;
  else
    array2[:] = 10;

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");

  x = atoi (argv[1])-10;
  y = atoi (argv[1])/2;
  z = (atoi (argv[1]))/5;

  for (ii = 0; ii < 10; ii++)
    {
      if (ii % 2)
	array[ii] = 0;
      else
	array[ii] = 1;
    }

  /*printf("x = %2d y = %2d z = %2d\n", x, y, z); */

  for (ii = 0; ii < 10; ii++)
    array[ii] = 10;

  /* This if loop will change all the 10's to 5's */
  if (array[x:y:z] != 9)
    array2[:] = 5;
  else
    array2[:] = 10;

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");

  for (ii = 0; ii < 10; ii++)
    array2[ii] = 10;

  /* This if loop will change all the 10's to 5's */
  if (array[atoi(argv[1])-10:atoi(argv[1]): atoi(argv[1])/5])
    array2[:] = 5;
  else
    array2[:] = 10;

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");

  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      TwodArray[ii][jj] = atoi(argv[1]);


  for (ii = 0; ii < 10; ii++)
    array2[ii] = 10;

  /* atoi(argv[1]) == 10, so it will convert all 10's to 5's */
  if (TwodArray[:][:] != 10) 
    array2[:] = 10; 
  else
    array2[:] = 5;

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");

  for (ii = 0; ii < 10; ii++)
    array2[ii] = 10;

  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      for (kk = 0; kk < 10; kk++)
	for (ll = 0; ll < 10; ll++) 
	  FourDArray[ii][jj][kk][ll] = atoi(argv[1]);
  
  /* atoi(argv[1]) == 10, so it will convert all 10's to 5's */
  if (FourDArray[:][:][:][:] != 10) 
    array2[:] = 10; 
  else
    array2[:] = 5;

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");

  
  for (ii = 0; ii < 10; ii++)
    array2[ii] = 10;

  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      for (kk = 0; kk < 10; kk++)
	for (ll = 0; ll < 10; ll++) 
	  FourDArray[ii][jj][kk][ll] = atoi(argv[1]);
  
  /* atoi(argv[1]) == 10, so it will convert all 10's to 5's */
  if (FourDArray[0:10:1][0:5:2][9:10:-1][x:y:z] != 10) 
    array2[:] = 10; 
  else
    array2[:] = 5;

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");

    for (ii = 0; ii < 10; ii++)
    array2[ii] = 10;

  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      for (kk = 0; kk < 10; kk++)
	for (ll = 0; ll < 10; ll++) 
	  FourDArray[ii][jj][kk][ll] = atoi(argv[1]);
  
  /* atoi(argv[1]) == 10, so it will convert all 10's to 5's */
  if (FourDArray[0:10:1][0:5:2][9:10:-1][x:y:z] +
      FourDArray[0:10:1][0:5:2][9:-10:1][x:y:z]  != 20) 
    array2[:] = 10; 
  else
    array2[:] = 5;

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");

  
  return 0;
}
