#include <iostream>
#include <cilk/cilk.h>
#include <cstdlib>

template <typename T>
void some_func(char *number)
{
  /* this shouldn't output an error */
  cilk_for (T i = 0; i < atoi (number); i += 1)
    std::cout << "Test += " << std::endl;
	 
  cilk_for (T j = atoi(number); j > 0 ; j -= 1)
    std::cout << "Test -=" << std::endl;
	 
  cilk_for (T k = 0; k < atoi (number); k++)
    std::cout << "Test ++" << std::endl;
	 
  cilk_for (T kk = atoi (number); kk > 0; kk--) 
    std::cout << "Test --" << std::endl;
	 
  std::cout << std::endl;
  return;
}
	 
int main(int argc, char **argv)
{
  if (argc == 1)
    return -1;
	 
  some_func<int>(argv[1]);
  some_func<char>(argv[1]);
  some_func<long>(argv[1]);
  some_func<unsigned char>(argv[1]);
  return 0;
}
