#include <stdio.h>
int main(int argc, char **argv)
{
  int array[10], array2[10], ii = 0, x = 2, z= 0 , y = 0 ;
  if (argc < 2)
    {
      fprintf(stderr,"Usage:%s <NUMBER>\n", argv[0]);
      return -1;
    }

  for (ii = 0; ii < 10; ii++)
    {
      array[ii] = 10;
      array2[ii] = 5000000;
    }

  array2[0:10:1] = array[0:10:1];

  printf("==============================================\n");
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %7d\tarray2[%2d] = %7d\n", ii, array[ii], 
	   ii, array2[ii]);

  for (ii = 0; ii < 10; ii++)
    {
      array[ii] = 10;
      array2[ii] = 5000000;
    }
  array2[0:10:2] = array[0:10:2];

  printf("==============================================\n");
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %7d\tarray2[%2d] = %7d\n", ii, array[ii], 
	   ii, array2[ii]);

  for (ii = 0; ii < 10; ii++)
    {
      array[ii] = 10;
      array2[ii] = 5000000;
    }
  x = atoi(argv[1]);
  z = (10-atoi(argv[1]))/atoi(argv[1]);
  printf("==============================================\n");
  printf("x = %2d\tz = %2d\n", x, z);
  array2[x:5:z] = array[x:5:z];
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %7d\tarray2[%2d] = %7d\n", ii, array[ii], 
	   ii, array2[ii]);

  for (ii = 0; ii < 10; ii++)
    {
      array[ii] = 500;
      array2[ii] = 1000000;
    }
  x = atoi(argv[1]);
  z = (10-atoi(argv[1]))/atoi(argv[1]);
  y = 10-atoi(argv[1]);
  printf("==============================================\n");
  printf("x = %2d\ty = %2d\tz = %2d\n", x, y, z);
  array2[x:y:z] = array[x:y:z];
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %7d\tarray2[%2d] = %7d\n", ii, array[ii], 
	   ii, array2[ii]);

  for (ii = 0; ii < 10; ii++)
    {
      array[ii]  = 500;
      array2[ii] = 1000000;
    }
  x = atoi(argv[1]);
  z = (10-atoi(argv[1]))/atoi(argv[1]);
  y = 10-atoi(argv[1]);
  printf("==============================================\n");
  printf("x = %2d\ty = %2d\tz = %2d\n", x, y, z);
  array[x:y:((10-atoi(argv[1]))/atoi(argv[1]))] = 
    array2[x:y:((10-atoi(argv[1]))/atoi(argv[1]))];
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %7d\tarray2[%2d] = %7d\n", ii, array[ii], 
	   ii, array2[ii]);

  x = atoi(argv[1]);
  z = (10-atoi(argv[1]))/atoi(argv[1]);
  y = 10-atoi(argv[1]);
  for (ii = 0; ii < 10; ii++)
    {
      array[ii]  = 500;
      array2[ii] = 1000000;
    }
  printf("==============================================\n");
  printf("x = %2d\ty = %2d\tz = %2d\n", x, y, z);
  array[atoi(argv[1]):(10-atoi(argv[1])):((10-atoi(argv[1]))/atoi(argv[1]))] =
  array2[atoi(argv[1]):(10-atoi(argv[1])):((10-atoi(argv[1]))/atoi(argv[1]))];
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %7d\tarray2[%2d] = %7d\n", ii, array[ii], 
	   ii, array2[ii]);

  for (ii = 0; ii < 10; ii++)
    {
      array[ii]  = 4;
      array2[ii] = 2;
    }

  array[atoi("5"):5:1] = array2[atoi("5"):5:1];
  printf("==============================================\n");
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %2d\tarray2[%2d] = %2d\n", ii, array[ii], 
	   ii, array2[ii]);

  for (ii = 0; ii < 10; ii++)
    {
      array[ii]  = 5;
      array2[ii] = 1;
    }
  array[atoi("5"):atoi("5"):atoi("1")] = 
			     array2[atoi("5"):atoi("5"):atoi("1")];
  printf("==============================================\n");
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %2d\tarray2[%2d] = %2d\n", ii, array[ii], 
	   ii, array2[ii]);

  return 0;
}
