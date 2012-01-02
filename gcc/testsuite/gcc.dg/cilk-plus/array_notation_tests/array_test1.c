#include <stdio.h>
int main(int argc, char **argv)
{
  int array[10], ii = 0, x = 2, z= 0 , y = 0 ;

  if (argc < 2)
    {
      fprintf(stderr,"Usage:%s <NUMBER>\n", argv[0]);
      return -1;
    }
  for (ii = 0; ii < 10; ii++)
    array[ii] = 10;

  array[0:10:1] = 15;

  printf("==============================================\n");
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %2d\n", ii, array[ii]);

  array[0:10:2] = 20;

  printf("==============================================\n");
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %2d\n", ii, array[ii]);


  x = atoi(argv[1]);
  z = (10-atoi(argv[1]))/atoi(argv[1]);
  printf("==============================================\n");
  printf("x = %2d\tz = %2d\n", x, z);
  array[x:5:z] = 50;
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %2d\n", ii, array[ii]);

  x = atoi(argv[1]);
  z = (10-atoi(argv[1]))/atoi(argv[1]);
  y = 10-atoi(argv[1]);
  printf("==============================================\n");
  printf("x = %2d\ty = %2d\tz = %2d\n", x, y, z);
  array[x:y:z] = 505;
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %2d\n", ii, array[ii]);

  x = atoi(argv[1]);
  z = (10-atoi(argv[1]))/atoi(argv[1]);
  y = 10-atoi(argv[1]);
  printf("==============================================\n");
  printf("x = %2d\ty = %2d\tz = %2d\n", x, y, z);
  array[x:y:((10-atoi(argv[1]))/atoi(argv[1]))] = 25;
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %2d\n", ii, array[ii]);

  x = atoi(argv[1]);
  z = (10-atoi(argv[1]))/atoi(argv[1]);
  y = 10-atoi(argv[1]);
  printf("==============================================\n");
  printf("x = %2d\ty = %2d\tz = %2d\n", x, y, z);
  array[atoi(argv[1]):(10-atoi(argv[1])):((10-atoi(argv[1]))/atoi(argv[1]))] =
					 1400;
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %2d\n", ii, array[ii]);

  array[atoi("5"):5:1] = 5555;
  printf("==============================================\n");
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %2d\n", ii, array[ii]);

  array[atoi("5"):atoi("5"):atoi("1")] = 9999;
  printf("==============================================\n");
  for (ii = 0; ii<10; ii++)
    printf("array[%2d] = %2d\n", ii, array[ii]);

  return 0;
}
