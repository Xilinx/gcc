#if HAVE_IO
#include <iostream>
#endif

#include <cstdlib>
#include <cassert>
int main(int argc, char **argv)
{
  int ii = 0, jj, kk, ll, array_ND[10][10][10][10], array_ND2[10][10][10][10];
  int x = 0, y = 10, z = 1;
  
  if (argc != 3)
    {
#if HAVE_IO
      std::cout << "Usage ./a.out 10 5" << std::endl;
#endif
      return 0;
    }

  array_ND[:][:][:][:] = 10;
#if HAVE_IO
  std::cout << "Modified " << std::endl;
  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      for (kk = 0; kk < 10; kk++)
	for (ll = 0; ll < 10; ll++)
	  std::cout << "\tArray_ND[" << ii << "][" << jj << "][" << kk 
		    << "][" << ll
		    << "] = " << array_ND[ii][jj][kk][ll] << std::endl;
#endif
  array_ND[:][:][:][:] = 5;
  array_ND2[:][:][:][:] = 10;
  
  /* This will replace all 10's to 5 */
  array_ND[0:10:1][atoi(argv[1])-10:atoi(argv[1]):atoi(argv[1])/10][:][:] =
    (10 * (atoi (argv[1])/atoi(argv[2])) - array_ND2[0:10:1][atoi(argv[1])-10:atoi(argv[1]):atoi(argv[1])/10][:][:])/2;

#if HAVE_IO
  std::cout << "Modified " << std::endl;
  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      for (kk = 0; kk < 10; kk++)
	for (ll = 0; ll < 10; ll++)
	  {
	    std::cout << "\tArray_ND[" << ii << "][" << jj << "][" << kk 
		      << "][" << ll
		      << "] = " << array_ND[ii][jj][kk][ll] << std::endl;
     
	    assert (array_ND[ii][jj][kk][ll] == 5);
	  }
#endif

  array_ND[:][:][:][:] = 5;
  array_ND2[:][:][:][:] = 10;
  /* This will replace all 10's to 5 */
  array_ND[0:10:1][atoi(argv[1])-10:atoi(argv[1]):atoi(argv[1])/10][9:10:-1][9:-10:1] =
    (10 * (atoi (argv[1])/atoi(argv[2])) - 
     array_ND2[0:10:1][atoi(argv[1])-10:atoi(argv[1]):atoi(argv[1])/10][:][:])/2;
#if HAVE_IO
  std::cout << "Modified " << std::endl;
  for (ii = 0; ii < 10; ii++)
    for (jj = 0; jj < 10; jj++)
      for (kk = 0; kk < 10; kk++)
	for (ll = 0; ll < 10; ll++)
	  {
	    std::cout << "\tArray_ND[" << ii << "][" << jj << "][" << kk 
		      << "][" << ll
		      << "] = " << array_ND[ii][jj][kk][ll] << std::endl;
	    assert (array_ND[ii][jj][kk][ll] == 5);
	  }
#endif
  return 0;
}
