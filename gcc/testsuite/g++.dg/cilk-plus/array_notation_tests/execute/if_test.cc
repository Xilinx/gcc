/* { dg-do run } */
/* { dg-options " " } */
#if HAVE_IO
#include <cstdio>
#endif
#include <cstdlib>
#include<string.h>
int main2 (int argc, char **argv);
int main(int argc, char **argv)
{
  int x = 0;
  if (argc == 1)
    {
      char *array[3];
      array[0] = strdup ("a.out");
      array[1] = strdup ("10");
      array[2] = strdup ("15");
      x = main2 (3, array);
    }
  else if (argc == 3)
    x = main2 (argc, argv);
  else
    abort ();
 
  return x;
}

void print_array (int *array)
{
#if HAVE_IO
  for (int ii = 0; ii < 10; ii++)
    printf("%d\n", array[ii]);
#endif
}

void print_2d_array (int array[10][10])
{
#if HAVE_IO
  for (int ii = 0; ii < 10; ii++) 
    for (int jj = 0; jj < 10; jj++) 
      printf("%d\n", array[ii][jj]);
  fflush (stdout);
#endif
}
int main2 (int argc, char **argv)
{
  int x = 3, y, z, array[10], array2[10], TwodArray[10][10], jj,kk,ll ;
  int array2d[10][10], array2d_check[10][10];
  int array2_check[10], array4_check[10][10][10][10];
  int FourDArray[10][10][10][10], array4d[10][10][10][10];
  int ii = 0; 

  x = atoi (argv[1])-10;
  y = atoi (argv[1])/2;
  z = (atoi (argv[1]))/5;
  for (ii = 0; ii < 10; ii++)
    {
      array[ii] = argc%3;
      array2[ii]= 10;
      array2_check[ii] = 10;
    }

  if (!array[:])
    array2[:] = 5;
  else
    array2[:] = 10;


  for (ii = 0; ii < 10; ii++)
    {
      if (!array[ii])
	array2_check[ii] = 5;
      else
	array2_check[ii] = 10;
    }

  print_array (array2);
  print_array (array2_check);
  #if HAVE_IO
  printf("1 ===================================================\n");
#endif
  for (ii = 0; ii < 10; ii++)
    if (array2_check[ii] != array2[ii]) {
      abort ();
    }

  if (!(array[0:10:1] + array[0:10:1]))
    array2[:] = 5;
  else
    array2[:] = 10;

  for (ii = 0; ii < 10; ii++)
    {
      if (!(array[ii]+ array[ii]))
	array2_check[ii] = 5;
      else
	array2_check[ii] = 10;
    }

  print_array (array2);
  print_array (array2_check);
  #if HAVE_IO
  printf("2.===================================================\n");
#endif
  for (ii = 0; ii < 10; ii++)
    if (array2_check[ii] != array2[ii])
      abort ();


  for (ii = 0; ii < 10; ii++)
    {
      if (ii % 2)
	array[ii] = 0;
      else
	array[ii] = 1;
    }

  for (ii = 0; ii < 10; ii++)
    array[ii] = 10;
  
  for (ii = 0; ii < 10; ii++)
    {
      array2[ii] = 10;
      array2_check[ii] = 10;
    }

  /* This if loop will change all the 10's to 5's */
  if (array[x:y:z] != 9)
    array2[x:y:z] = 5;
  else
    array2[x:y:z] = 10;

  for (ii = 0; ii < 10; ii += z)
    {
      if (array[ii] != 9)
	array2_check[ii] = 5;
      else
	array2_check[ii] = 10;
    }

  print_array (array2);
  print_array (array2_check);
  #if HAVE_IO
  printf("3.===================================================\n");
#endif
  for (ii = 0; ii < 10; ii++)
    if (array2_check[ii] != array2[ii])
      abort ();
  for (ii = 0; ii < 10; ii++) {
    array2[ii] = 10;
    array2_check[ii] = 10;
  }
 
  if (array[atoi(argv[1])-10:atoi(argv[1])/2: atoi(argv[1])/5])
    array2[0:5:2] = 5;
  else
    array2[0:5:2] = 10;

  for (ii = atoi(argv[1])-10; ii < atoi(argv[1]) + (atoi (argv[1])-10);
       ii +=atoi(argv[1])/5)
    if (array[ii])
      array2_check[ii] = 5;
    else
      array2_check[ii] = 10;

  for (ii = 0; ii < 10; ii++)
    if (array2_check[ii] != array2[ii])
      abort ();

  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      TwodArray[ii][jj] = atoi(argv[1]);


  for (ii = 0; ii < 10; ii++)
    {
      array2[ii] = 10;
      array2_check[ii] = 10;
    }

  if (TwodArray[:][:] != 10) 
    array2d[:][:] = 10; 
  else
    array2d[:][:] = 5;

  for (ii = 0; ii < 10; ii++) {
    for (jj = 0; jj < 10; jj++) {
      if (TwodArray[ii][jj] != 10)
	array2d_check[ii][jj] = 10;
      else
	array2d_check[ii][jj] = 5;
    }
  }
  
  print_2d_array (array2d);
  print_2d_array (array2d_check);
#if HAVE_IO
  printf("4. ===================================================\n");
#endif
  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      if (array2d_check[ii][jj] != array2d[ii][jj])
	abort ();

  for (ii = 0; ii < 10; ii++)
    {
      array2[ii] = 10;
      array2_check[ii] = 10;
    }

  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      for (kk = 0; kk < 10; kk++)
	for (ll = 0; ll < 10; ll++) 
	  { 
	    FourDArray[ii][jj][kk][ll] = atoi(argv[1]);
	    array4d[ii][jj][kk][ll] = 10;
	    array4_check[ii][jj][kk][ll] = 10;
	  }

  if (FourDArray[:][:][:][:] != 10) 
    array4d[:][:][:][:] = 10; 
  else
    array4d[:][:][:][:] = 5;
  for (ii = 0; ii < 10; ii++) {
    for (jj = 0; jj < 10; jj++) {
      for (kk = 0; kk < 10; kk++) {
	for (ll = 0; ll < 10; ll++) {
	  if (FourDArray[ii][jj][kk][ll] != 10)
	    array4_check[ii][jj][kk][ll] = 10;
	  else
	    array4_check[ii][jj][kk][ll] = 5;
	}
      }
    }
  }
  
#if HAVE_IO
  printf("5. ===================================================\n");
#endif
 for (ii = 0; ii < 10; ii++)
  for (jj = 0; jj < 10; jj++)
   for (kk = 0; kk < 10; kk++)
    for (ll = 0; ll < 10; ll++)
     if (array4_check[ii][jj][kk][ll] != array4d[ii][jj][kk][ll])
      abort ();

  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      for (kk = 0; kk < 10; kk++)
	for (ll = 0; ll < 10; ll++) 
	  FourDArray[ii][jj][kk][ll] = atoi(argv[1]);
  
  if (FourDArray[0:10:1][0:5:2][9:10:-1][x:y:z] != 10) 
    array4d[0:10][0:5:2][9:10:-1][0:5:2] = 10; 
  else
    array4d[0:10][0:5:2][9:10:-1][x:y:z] = 5;

  for (ii = 0; ii < 10; ii++) {
    for (jj = 0; jj < 10; jj += 2) {
      for (kk = 9; kk >= 0; kk--) {
	for (ll = x; ll < 10; ll = ll += z) {
	  if (FourDArray[ii][jj][kk][ll] != 10)
	    array4_check[ii][jj][kk][ll] = 10;
	  else
	    array4_check[ii][jj][kk][ll] = 5;
	}
      }
    }
  }

  for (ii = 0; ii < 10; ii++)
    {
      array2[ii] = 10;
      array2_check[ii] = 10;
    }

  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      for (kk = 0; kk < 10; kk++)
	for (ll = 0; ll < 10; ll++) 
	  FourDArray[ii][jj][kk][ll] = atoi(argv[1]);
  
  /* atoi(argv[1]) == 10, so it will convert all 10's to 5's */
  if (FourDArray[0:10:1][0:5:2][9:10:-1][x:y:z] +
      FourDArray[0:10:1][0:5:2][9:-10:1][x:y:z]  != 20) 
    array4d[0:10][0:5:2][9:10:-1][x:y:z] = 10; 
  else
    array4d[:][0:5:2][9:-10:1][x:y:z] = 5;

  for (ii = 0; ii < 10; ii++) {
    for (jj = 0; jj < 10; jj += 2) {
      for (kk = 9; kk >= 0; kk--) {
	for (ll = x; ll < 10; ll = ll += z) {
	  if (FourDArray[ii][jj][kk][ll] != 10)
	    array4_check[ii][jj][kk][ll] = 10;
	  else
	    array4_check[ii][jj][kk][ll] = 5;
	}
      }
    }
  }
  for (ii = 0; ii < 10; ii++) {
    for (jj = 0; jj < 10; jj += 2) {
      for (kk = 9; kk >= 0; kk--) {
	for (ll = x; ll < 10; ll = ll += z) {
	  if (array4_check[ii][jj][kk][ll] != array4d[ii][jj][kk][ll])
	    abort ();
	}
      }
    }
  }
  return 0;
}
