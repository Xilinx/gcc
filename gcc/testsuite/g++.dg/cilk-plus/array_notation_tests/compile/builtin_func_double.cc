#if HAVE_IO
#include <iostream>
using namespace std;
#endif
#include <stdlib.h>

/* char __sec_reduce_add (int *); */
int main(int argc, char **argv)
{
  int ii,array[10], y = 0, y_int = 0, array2[10];
  double x, yy, array3[10], array4[10];
  for (ii = 0; ii < 10; ii++)
    {
      array[ii] = 1+ii;
      array2[ii]= 2; 
    }

  for (ii = 0; ii < 10; ii++)
    {
      if (ii%2 && ii)
	array3[ii] = (double)(1.0000/(double)ii);
      else
	array3[ii] = (double) ii + 0.10;
      array4[ii] = (double) (1.00000/ (double)(ii+1));
    }

  /* array[:] = 5; */
  x = __sec_reduce_max (array3[:] * array4[:]); 
  y = __sec_reduce_max_ind ( array3[:] * array4[:]); 
#if HAVE_IO
  for (ii = 0; ii < 10; ii++) 
    cout << array3[ii] * array4[ii] << " ";
  cout << endl;
  cout << "Max = " << x << " Max Index = " << y << endl;
#endif

  x = __sec_reduce_min (array3[:] * array4[:]); 
  y = __sec_reduce_min_ind ( array3[:] * array4[:]); 
#if HAVE_IO
  for (ii = 0; ii < 10; ii++) 
    cout << array3[ii] * array4[ii] << " ";
  cout << endl;
  cout << "Min = " << x << " Min Index = " << y << endl;
#endif

  x = __sec_reduce_add (array3[:] * array4[:]); 
  yy = __sec_reduce_mul ( array3[:] * array4[:]); 
#if HAVE_IO
  for (ii = 0; ii < 10; ii++) 
    cout << array3[ii] * array4[ii] << " ";
  cout << endl;
  cout << "Add = " << x << " Mul = " << yy << endl;
#endif

  for (ii = 0; ii < 10; ii++)
    {
      if (ii%2 && ii)
	array3[ii] = (double)(1.0000/(double)ii);
      else
	array3[ii] = (double) ii + 0.00;
      array4[ii] = (double) (1.00000/ (double)(ii+1));
    }
  y_int = __sec_reduce_any_zero (array3[:] * array4[:]); 
  y = __sec_reduce_all_zero ( array3[:] * array4[:]); 
#if HAVE_IO
  for (ii = 0; ii < 10; ii++) 
    cout << array3[ii] * array4[ii] << " ";
  cout << endl;
  cout << "Any Zeros = " << y_int << " All Zeros = " << y << endl;
#endif
  return 0;
}
