#if HAVE_IO
#include <iostream> 
#endif

extern int funct(int x);
extern int funct2(int x, int y);
extern void func_ptr (int *x);
int main(int argc, char **argv)
{
  int array[10], array2[10], x = 0, y;
  for (int ii = 0; ii < 10; ii++)
    array[ii] = 0;

  array[:]++;
#if HAVE_IO
  for (int ii = 0; ii < 10; ii++)
    array[0:10:1]++;
#endif

#if HAVE_IO
  std::cout << "Before Function Call" << std::endl;
  for (int ii = 0; ii < 10; ii++)
    std::cout << array[ii] << std::endl;
#endif

  array2[:] = funct(array[:]);
  
#if HAVE_IO
  std::cout << std::endl;
  std::cout << "After Function Call" << std::endl;
  for (int ii = 0; ii < 10; ii++)
    std::cout << array2[ii] << std::endl;
#endif

  x += funct (array[:]);
#if HAVE_IO
  std::cout << "x (Just sum of all in After Function Call) = " << x 
    << std::endl;
#endif

  x = 0;
  x += funct (array[0:10:1]);
#if HAVE_IO
  std::cout << "x (Just sum of all in After Function Call) = " << x 
    << std::endl;
#endif

  x = 0;
  x += funct (array[0:10/argc:argc]);
#if HAVE_IO
  std::cout << "x (Just sum of all in After Function Call, if Argc == 1) = " 
    << x << std::endl;
#endif

  x = 0;
#if HAVE_IO
  x = funct (funct(array[:]));
  std::cout << "x (Just array[9]+1+1) = " << x << std::endl;
#endif
  array2[:] = funct (funct (funct (array[:])));

#if HAVE_IO
  std::cout << "After Function Call (must be array[x]+1+1+1" << std::endl;
  for (int ii = 0; ii < 10; ii++)
    std::cout << array2[ii] << std::endl;
#endif
  x = 0;
  for (int ii = 0; ii < 10; ii++) 
    x += funct (funct (funct (array[:])));

#if HAVE_IO
  std::cout << "x  (After 3 function calls in for loop 14 * 100) =  " << x <<
    std::endl;
#endif
  x = 0;
  for (int ii = 0; ii < 10; ii++)
    x += funct (funct (funct (funct2 (array[:], array2[:]))));

#if HAVE_IO
  std::cout << 
    "x  (After 4 different function calls in for loop ((11+14)+3) * 100) =  " << x <<
    std::endl;
#endif

#if HAVE_IO
  std::cout << std::endl;
  std::cout << "Before adding every element by 1" << std::endl;
  for (int ii = 0; ii < 10; ii++)
    std::cout << array[ii] << std::endl;
#endif

  func_ptr (&array[:]);
#if HAVE_IO
  std::cout << std::endl;
  std::cout << "After adding every element by 1" << std::endl;
  for (int ii = 0; ii < 10; ii++)
    std::cout << array[ii] << std::endl;
#endif
  return x;
}
