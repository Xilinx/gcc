#if HAVE_IO
#include <iostream>
#endif

#include <cstdlib>
int main(int argc, char **argv)
{
  int ii = 0, jj, kk, ll, array[10],array2[10], array_ND[10][10][10][10];
  int x = 0, y = 10, z = 1;
  
  if (argc != 3)
    {
#if HAVE_IO
      std::cout << "Usage: ./a.out 10 5" << std::endl;
#endif
      return 0;
    }
  array[:] = 10; 
#if HAVE_IO
  std::cout << "Original " << std::endl;
  for (ii = 0; ii < 10; ii++)
    std::cout << "\tArray1[" << ii << "] = " << array[ii] << std::endl;
#endif

  /* This if loop will change all the 10's to 5's */
  array[:] = atoi (argv[2]);
#if HAVE_IO
  std::cout << "Modified " << std::endl;
  for (ii = 0; ii < 10; ii++)
    std::cout << "\tArray1[" << ii << "] = " << array[ii] << std::endl;
#endif
  array[:] = 10; 
#if HAVE_IO
  std::cout << "Original " << std::endl;
  for (ii = 0; ii < 10; ii++)
    std::cout << "\tArray1[" << ii << "] = " << array[ii] << std::endl;
#endif
  /* This if loop will change all the 10's to 5's */
  array[atoi(argv[1])-10:atoi(argv[1]):atoi(argv[1])/10] = 5;
#if HAVE_IO
  std::cout << "Modified " << std::endl;
  for (ii = 0; ii < 10; ii++)
    std::cout << "\tArray1[" << ii << "] = " << array[ii] << std::endl;
#endif
  array[:] = 10; 
#if HAVE_IO
  std::cout << "Original " << std::endl;
  for (ii = 0; ii < 10; ii++)
    std::cout << "\tArray1[" << ii << "] = " << array[ii] << std::endl;
#endif
  /* This if loop will change all the 10's to 5's */
  array[atoi(argv[1])-10:atoi(argv[1]):atoi(argv[1])/atoi(argv[2])] = 
    atoi (argv[1]) / atoi (argv[2]) + argc;
#if HAVE_IO
  std::cout << "Modified " << std::endl;
  for (ii = 0; ii < 10; ii++)
    std::cout << "\tArray1[" << ii << "] = " << array[ii] << std::endl;
#endif
  array[:] = 10; 
#if HAVE_IO
  std::cout << "Original " << std::endl;
  for (ii = 0; ii < 10; ii++)
    std::cout << "\tArray1[" << ii << "] = " << array[ii] << std::endl;
#endif
  /* This if loop will change all the 10's to 5's */
  array[atoi(argv[1])-10:atoi(argv[1]):atoi(argv[1])/atoi(argv[2])] = 
    atoi (argv[1]) / atoi (argv[2]) + 3;
#if HAVE_IO
  std::cout << "Modified " << std::endl;
  for (ii = 0; ii < 10; ii++)
    std::cout << "\tArray1[" << ii << "] = " << array[ii] << std::endl;
#endif
  array[:] = 10; 
  array2[:] = 10; 
#if HAVE_IO
  std::cout << "Original " << std::endl;
  for (ii = 0; ii < 10; ii++)
    {
      std::cout << "\tArray1[" << ii << "] = " << array[ii];
      std::cout << "\tArray2[" << ii << "] = " << array2[ii] << std::endl;
    }
#endif
  /* This if loop will change all the 10's to 5's */
  array[atoi(argv[1])-10:atoi(argv[1]):atoi(argv[1])/10] = 
    (25 * (array2[atoi(argv[1])-10:atoi(argv[1]):atoi(argv[1])/10] -
	   (atoi (argv[1]) / atoi (argv[2]) + argc)))/ atoi ("25");

  return 0;
}
