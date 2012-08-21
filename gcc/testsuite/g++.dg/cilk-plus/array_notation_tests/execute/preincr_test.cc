/* { dg-do run } */
/* { dg-options " " } */


#include <cstdlib>

int main(int argc, char **argv)
{
  int array[10], array_serial[10];

  for (int ii = 0; ii < 10; ii++) {
    array[ii] = 0;
    array_serial[ii] = 0;
  }

  array[:] = 19383;
  for (int ii = 0; ii < 10; ii++) 
    array_serial[ii] = 19383;

  ++array[:];

  for (int ii = 0; ii < 10; ii++)
    ++array_serial[ii];

  for (int ii = 0; ii < 10; ii++)
    if (array_serial[ii] != array[ii])
      abort ();

  for (int ii = 0; ii < 10; ii++)
    ++array[:];

  for (int ii = 0; ii < 10; ii++)
    for (int jj = 0; jj < 10; jj++)
      ++array_serial[jj];

  for (int ii = 0; ii < 10; ii++)
    if (array_serial[ii] != array[ii])
      abort ();

  if (argc == 2)
    ++array[0:10:1]; 

  if (argc == 2)
    {
      for (int ii = 0; ii < 10; ii++)
	++array_serial[ii];
    }

  for (int ii = 0; ii < 10; ii++)
    if (array_serial[ii] != array[ii])
      abort ();

  ++array[0:10/argc:argc]; 
  
  for (int ii = 0; ii < 10; ii += argc)
    {
      ++array_serial[ii];
    }

  for (int ii = 0; ii < 10; ii++)
    if (array_serial[ii] != array[ii])
      abort ();

  return 0;
}
