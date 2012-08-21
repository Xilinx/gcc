#if HAVE_IO
#include <iostream> 
#endif
/* #include <cstdlib> */

int main(int argc, char **argv)
{
  int array[10], array2[10], x = 0, y;
  float array3[10][10][10];
  int x_correct, y_correct;

  for (int ii = 0; ii < 10; ii++)
    {
      array[ii] = 1+ii;
      array2[ii]= 2;
    }
  
  array[array2[:]] = 5;
#if HAVE_IO
  for (int ii = 0; ii < 10; ii++)
    std::cout << "array[" << ii << "] = " << array[ii] << std::endl;
#endif

  array3[array[:]][array2[:]][array[0:10:1]] = (float)array[array2[:]];

#if HAVE_IO
  for (int ii = 0; ii < 10; ii++)
    for (int jj = 0; jj < 10; jj++)
      for (int kk = 0; kk < 10; kk++)
	std::cout << "array[" << ii << "][" << jj << "][" << kk << "] = "
	  << array3[ii][jj][kk] << std::endl;
#endif

  return 0;
}
