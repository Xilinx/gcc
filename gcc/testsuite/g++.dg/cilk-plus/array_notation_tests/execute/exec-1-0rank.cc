/* { dg-do run } */
/* { dg-options " -std=c++11 " } */

#if HAVE_IO
#include <iostream>
#endif
#define N 1500

class Data {
  int run = 0;
public:
  int foo () { return ++run; }
} D;

int A[N];

int main () {
  int i;

  /* Check exec-once scenario when  RHS rank is zero.  */
  A[:] = D.foo ();
#if HAVE_IO
  for (i = 0; i < N; i++) 
    std::cout << A[i] << " ";
  std::cout << std::endl;
#endif
  for (i = 0; i < N; i++)
    if (A[i] != 1)
      return 1;

  return 0;
}
