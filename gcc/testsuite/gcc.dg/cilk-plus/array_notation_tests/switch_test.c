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
#if 1
  switch (array[:])
    {
    case 10:
      array2[:] = 0;
      break;
    case 9:
      array2[:] = 1;
      break;
    case 0:
      array2[:] = 5;
      break;
    case 1:
      array2[:] = 10;
      break;
    default:
      array2[:] = 6;
    }

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");
#endif
  
#if 1
  /* this tests for nested if statements */
  if (!(array[0:10:1] + array[0:10:1]))
    if ((array[0:10:1] + atoi(argv[1])))
      array2[:] = 5;
    else
      array2[:] = 7;
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

#endif

  /*printf("x = %2d y = %2d z = %2d\n", x, y, z); */
#if 1
  for (ii = 0; ii < 10; ii++)
    array[ii] = 10;

  /* This if loop will change all the 10's to 5's */
  switch (array[x:y:z])
    {
    case 9:
      array2[:] = 10;
      break;
    default:
      array2[:] = 5;
      break;
    }

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");
#endif
  
#if 1
  for (ii = 0; ii < 10; ii++)
    array2[ii] = 10;

  /* This if loop will change all the 10's to 5's */
  switch (array[atoi(argv[1])-10:atoi(argv[1]): atoi(argv[1])/5])
    {
    case 10:
      array2[:] = 5;
      break;
    default:
      array2[:] = 10;
      break;
    }
  
  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");
#endif

#if 1
  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      TwodArray[ii][jj] = atoi(argv[1]);


  for (ii = 0; ii < 10; ii++)
    array2[ii] = 10;

  /* atoi(argv[1]) == 10, so it will convert all 10's to 5's */
  switch (TwodArray[:][:])
    {
    case 10:
      array2[:] = 5;
      break;
    default:
      array2[:] = 5 + array2[:];
      break;
    }

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");
#endif
  
#if 1
  for (ii = 0; ii < 10; ii++)
    array2[ii] = 10;

  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      for (kk = 0; kk < 10; kk++)
	for (ll = 0; ll < 10; ll++) 
	  FourDArray[ii][jj][kk][ll] = atoi(argv[1]);
  
  /* atoi(argv[1]) == 10, so it will convert all 10's to 5's */
  switch (FourDArray[:][:][:][:])
    {
    case 10:
      array2[:] = 5;
      break;
    default:
      array2[:] = 10;
      break;
    }

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");
#endif
  
#if 1  
  for (ii = 0; ii < 10; ii++)
    array2[ii] = 10;

  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      for (kk = 0; kk < 10; kk++)
	for (ll = 0; ll < 10; ll++) 
	  FourDArray[ii][jj][kk][ll] = atoi(argv[1]);
  
  /* atoi(argv[1]) == 10, so it will convert all 10's to 5's */
  switch (FourDArray[0:10:1][0:5:2][9:10:-1][x:y:z]) {
  case 5:
    array2[:] = 10;
    break;
  case 10:
    array2[:] = 5;
    break;
  default:
    array2[:] = 9;
    break;
  }

  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");

#endif
  
#if 1
    for (ii = 0; ii < 10; ii++)
    array2[ii] = 10;

  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      for (kk = 0; kk < 10; kk++)
	for (ll = 0; ll < 10; ll++) 
	  FourDArray[ii][jj][kk][ll] = atoi(argv[1]);
  
  /* atoi(argv[1]) == 10, so it will convert all 10's to 5's */
  switch(FourDArray[0:10:1][0:5:2][9:10:-1][x:y:z] +
	 FourDArray[0:10:1][0:5:2][9:-10:1][x:y:z]) {
  case 10:
    array2[:] = 10;
    break;
  case 15:
    array2[:] = 132;
    break;
  case 20:
    array2[:] = 5;
    break;
  default:
    array2[:] = 2;
    break;
  }
  for (ii = 0; ii < 10; ii++)
    printf("%4d ", array2[ii]);
  printf("\n");
#endif
  
  return 0;
}
