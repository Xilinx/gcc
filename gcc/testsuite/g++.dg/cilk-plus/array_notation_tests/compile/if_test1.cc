#if HAVE_IO
#include <iostream>
#include <cstdlib> 
#endif
int main(int argc, char **argv)
{
  int array[10], x;
#if  1
  for (int ii = 0; ii < 10; ii++)
    array[ii] = 0;

  array[:] = 19383;
#endif
  if (array[1] != 0) {
  if (array[2] != 0) {
  if (array[:] != 0) {
  if (array[0:5:1] != 0) {
  if (argc) {
  if (array[:] == 19383)
#if HAVE_IO
    std::cout << " 5" << std::endl;
#else
    x = 5;
#endif
  else
#if HAVE_IO
    std::cout << "10" << std::endl;
#else
    x = 10;
#endif
  
    }
  }
  }
  }
  }
  return x;
}
