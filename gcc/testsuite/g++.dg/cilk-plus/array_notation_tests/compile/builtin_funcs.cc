#if HAVE_IO
#include <iostream> 
#endif
#include <cstdlib>

extern int funct(int x);
extern int funct2(int x, int y);
extern void func_ptr (int *x);

/* char __sec_reduce_add (int *); */
int main(int argc, char **argv)
{
  int array[10], array2[10], x = 0, y;
  int x_correct, y_correct;

  for (int ii = 0; ii < 10; ii++)
    {
      array[ii] = 1+ii;
      array2[ii]= 2;
    }

  x = __sec_reduce_max (array[:] + array2[:]);
  y = __sec_reduce_max_ind (array[:] + array2[:]);
#if HAVE_IO
  std::cout <<  "Max = " << x << " Max Index = " << y << std::endl;
#endif

  x = __sec_reduce_max (array[0:5:1] + array2[5:5:1]);
  y = __sec_reduce_max_ind (array[0:5] + array2[5:5:1]);
#if HAVE_IO
  std::cout <<  "Max = " << x << " Max Index = " << y << std::endl;
#endif

  x = __sec_reduce_min (array[:] + array2[:]);
  y = __sec_reduce_min_ind (array[:] + array2[:]);
#if HAVE_IO
  std::cout <<  "Min = " << x << " Min Index = " << y << std::endl;
#endif

  x = __sec_reduce_min (array[0:5:1] + array2[5:5:1]);
  y = __sec_reduce_min_ind (array[0:5] + array2[5:5:1]);
#if HAVE_IO
  std::cout <<  "Min = " << x << " Min Index = " << y << std::endl;
#endif
  
  x = __sec_reduce_add (array[:] + array2[:]);
  y = __sec_reduce_mul (array[:] + array2[:]);

  x_correct = 0;
  y_correct = 1;
  for (int ii = 0; ii < 10; ii++)
    {
      x_correct += (array[ii] + array2[ii]);
      y_correct *= (array[ii] + array2[ii]);
    }
#if HAVE_IO
  std::cout <<  "Add          = " << x 
    << " Multiply           = " << y << std::endl;
  std::cout <<  "Add(Correct) = " << x_correct 
    << " Multiply (Correct) = " << y_correct << std::endl;
#endif

  x = __sec_reduce_add (array[0:5:1] + array2[5:5:1]);
  y = __sec_reduce_mul (array[0:5]   + array2[5:5:1]);
  
  x_correct = 0;
  y_correct = 1;
  for (int ii = 0; ii < 5; ii++)
    {
      x_correct += (array[ii] + array2[5+ii]);
      y_correct *= (array[ii] + array2[5+ii]);
    }
#if HAVE_IO
  std::cout <<  "Add          = " << x 
    << " Multiply           = " << y << std::endl;
  std::cout <<  "Add(Correct) = " << x_correct 
    << " Multiply (Correct) = " << y_correct << std::endl;
#endif

    array[:] = argc - atoi ("1");

    for (int ii = 0; ii < 10; ii++)
      array2[ii] = argc - ii;

  x = __sec_reduce_all_zero (array[0:5:1]);
  y = __sec_reduce_any_zero (array2[:]);
#if HAVE_IO
  std::cout <<  "All Zeros = " << x << " Any Zeros = " << y << std::endl;
#endif

  return 0;
}
