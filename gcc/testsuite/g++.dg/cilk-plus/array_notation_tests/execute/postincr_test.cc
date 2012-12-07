/* { dg-do run } */
/* { dg-options "" } */

#define HAVE_IO 1

#if HAVE_IO
#include <cstdio>
#endif

#include <cstdlib>
template <class T> int main2(int argc, char **argv);

int main (int argc, char **argv)
{
    return (main2<int>(argc,argv) + main2<long> (argc, argv) +
	              main2<long long> (argc, argv));
}

#if HAVE_IO
template <class T> int print_array (T *array, int size);
template <class T> int print_array (T *array, int size)
{
    for (int ii = 0; ii < size; ii++)
          printf("%d ", array[ii]);
      printf("\n");
        return 0;
}
#endif

template <class T>
int main2(int argc, char **argv)
{
  T array[10], array_serial[10];

  for (int ii = 0; ii < 10; ii++) {
    array[ii] = 0;
    array_serial[ii] = 0;
  }

  array[:] = 19383;
  for (int ii = 0; ii < 10; ii++)
    array_serial[ii] = 19383;

  array[:]++;
  
  for (int ii = 0; ii < 10; ii++)
    array_serial[ii]++;

#if HAVE_IO
  print_array<T>(array, 10);
  print_array<T>(array_serial, 10);
#endif

  for (int ii  = 0; ii < 10; ii++)
    if (array_serial[ii] != array[ii])
      abort ();

  for (int ii = 0; ii < 10; ii++)
    array[:]++;

  for (int ii = 0; ii < 10; ii++)
    for (int jj = 0; jj < 10; jj++)
      array_serial[jj]++;

#if HAVE_IO
  print_array<T>(array, 10);
  print_array<T>(array_serial, 10);
#endif

  for (int ii  = 0; ii < 10; ii++)
    if (array_serial[ii] != array[ii])
      abort ();

  if (argc == 2)
    array[0:10:1]++; 
  
  if (argc == 2)
    {
      for (int ii = 0; ii < 10; ii++)
	array_serial[ii]++;
    }

#if HAVE_IO
  print_array<T>(array, 10);
  print_array<T>(array_serial, 10);
#endif

  for (int ii  = 0; ii < 10; ii++)
    if (array_serial[ii] != array[ii])
      abort ();

  array[0:10/argc:argc]++; 
  
  for (int ii = 0; ii < 10; ii += argc) 
    array_serial[ii]++;


  for (int ii  = 0; ii < 10; ii++)
    if (array_serial[ii] != array[ii])
      abort ();

#if HAVE_IO
  print_array<T>(array, 10);
  print_array<T>(array_serial, 10);
#endif

  return 0;
}
