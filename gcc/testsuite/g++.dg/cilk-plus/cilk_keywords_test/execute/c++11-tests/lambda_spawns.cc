/* { dg-do run } */
/* { dg-options "-std=c++11" } */
#define FIRST_NUMBER 5
#define SECOND_NUMBER 3
#define HAVE_IO 0
#if HAVE_IO
#include <stdio.h>
#endif

#include <stdlib.h>

void foo1(int *array, int size)
{
#if HAVE_IO
  for (int ii = 0; ii < size; ii++) 
    printf("%2d\t", array[ii]);
  printf("\n");
  fflush (stdout);
#else
  if (size != 2)
    abort ();
  if (array[0] != FIRST_NUMBER)
    abort ();
  if (array[1] != SECOND_NUMBER)
    abort ();
#endif
}
void foo1_c(const int *array, int size)
{
#if HAVE_IO
  for (int ii = 0; ii < size; ii++) 
    printf("%2d\t", array[ii]);
  printf("\n");
  fflush (stdout);
#else
  if (size != 2)
    abort ();
  if (array[0] != FIRST_NUMBER)
    abort ();
  if (array[1] != SECOND_NUMBER)
    abort ();
#endif
}

int main (int argc, char **argv) {
  int A[2] = {FIRST_NUMBER, SECOND_NUMBER};
  int main_size = argc+1; /* We know argc is 1, and so 1+1 = 2.  */
  auto func0 = [=](){ foo1_c(A, 2); };
  _Cilk_spawn func0();
  foo1 (A, 2);
  _Cilk_sync;

  auto func1 = [=](int *Aa){ foo1(Aa, 2); };
  _Cilk_spawn func1 (A);
  foo1 (A, 2);
  _Cilk_sync;

  auto func2 = [=](int *Aa, int size){ foo1(Aa, size); };
  _Cilk_spawn func2 (A, 2);
  foo1 (A, 2);
  _Cilk_sync;

  auto func3 = [=](int *Aa, int size){ int new_size = (size % 2 + 2); 
				       foo1(Aa, size); };
  _Cilk_spawn func3 (A, 2);
  foo1 (A, 2);
  _Cilk_sync;

  auto func4 = [](int *Aa){ foo1(Aa, 2); };
  _Cilk_spawn func4 (A);
  foo1 (A, 2);
  _Cilk_sync;

  auto func5 = [](int *Aa, int size){ foo1(Aa, size); };
  _Cilk_spawn func5 (A, 2);
  foo1 (A, 2);
  _Cilk_sync;

  auto func6 = [&](int *Aa){ foo1(Aa, 2); };
  _Cilk_spawn func6 (A);
  foo1 (A, 2);
  _Cilk_sync;

  auto func7 = [&](int *Aa, int size){ foo1(Aa, size); };
  _Cilk_spawn func7 (A, 2);
  foo1 (A, 2);
  _Cilk_sync;

  auto func8 = [&](){ foo1(A, 2); };
  _Cilk_spawn func8 ();
  foo1 (A, 2);
  _Cilk_sync;

  /* We ignore the first param here and pass in A from the outer fn.  */
  auto func9 = [&](int *Aa, int size){ foo1(A, size); };
  _Cilk_spawn func9 (A, 2);
  foo1 (A, 2);
  _Cilk_sync;

  auto func10 = [=](){ foo1_c(A, main_size); };
  _Cilk_spawn func10 ();
  foo1 (A, 2);
  _Cilk_sync;

  auto func11 = [&](){ foo1(A, main_size); };
  _Cilk_spawn func11 ();
  foo1 (A, 2);
  _Cilk_sync;

  /* We ignore the first & second param here and pass in A from the 
     outer fn.  */
  auto func12 = [&](int *Aa, int size){ foo1(A, main_size); };
  _Cilk_spawn func12 (A, 2);
  foo1 (A, 2);
  _Cilk_sync;

  _Cilk_spawn [&](int *Aa){ foo1(Aa, 2); }(A);
  foo1 (A, 2);
  _Cilk_sync;

  _Cilk_spawn [&](int *Aa, int size){ foo1(Aa, size); }(A, 2);
  foo1 (A, 2);
  _Cilk_sync;

  _Cilk_spawn [=](int *Aa){ foo1(Aa, 2); }(A);
  foo1 (A, 2);
  _Cilk_sync;

  _Cilk_spawn [=](int *Aa, int size){ foo1(Aa, size); }(A, 2);
  foo1 (A, 2);
  _Cilk_sync;

  /* We ignore the first param here.  */
  _Cilk_spawn [=](int *Aa, int size){ foo1_c(A, size); }(A, 2);
  foo1 (A, 2);
  _Cilk_sync;

  /* We ignore the first and second param here.  */
  _Cilk_spawn [=](int *Aa, int size){ foo1_c(A, main_size); }(A, 2);
  foo1 (A, 2);
  _Cilk_sync;

  _Cilk_spawn [&](){ foo1(A, 2); }();
  [&](){ foo1(A, 2); }();
  _Cilk_sync;

  _Cilk_spawn [=](){ foo1_c(A, main_size); }();
  foo1 (A, 2);
  _Cilk_sync;
	
  _Cilk_spawn [&](){ foo1(A, main_size); }();
  [&](){ foo1(A, 2); }();
  _Cilk_sync;

  return 0;
}
