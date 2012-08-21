#if HAVE_IO
#include <iostream>
#include <cstdlib>
#endif
using namespace std;
int main(int argc, char **argv)
{
  int array[10];

  for (int ii = 0; ii < 10; ii++)
    array[ii] = 0;

  array[:] = 19383;

  array[:]--;

#if HAVE_IO
  for (int ii = 0; ii < 10; ii++)
    cout << array[ii] << endl;
  cout << endl;
#endif

  for (int ii = 0; ii < 10; ii++)
    array[:]--;

#if HAVE_IO
  for (int ii = 0; ii < 10; ii++)
    cout << array[ii] << endl;
  cout << endl;
#endif

  if (argc == 2)
    array[0:10:1]--; 

#if HAVE_IO
  for (int ii = 0; ii < 10; ii++)
    cout << array[ii] << endl;
  cout << endl;
#endif
  array[0:10/argc:argc]--; 

#if HAVE_IO
  for (int ii = 0; ii < 10; ii++)
    cout << array[ii] << endl;
#endif
  return 1;
}
