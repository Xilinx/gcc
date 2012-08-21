/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */

#include <cstdlib>
#include <cilk/cilk.h>

#define SIZE 3

int main() {
  int array[SIZE];
  int array_serial[SIZE];

  for (int *p = array; p < array + SIZE; ++p) {
    *p = 2;
  }
  for (int *p = array_serial; p < array_serial + SIZE; ++p) {
    *p = 2;
  }

  cilk_for (int *p = array ; p < array + SIZE; p += 2) {
    *p = 16;
  }

  cilk_for (int *p = array+SIZE-1 ; p >= array;  p -= 2) {
    *p = 9;
  }

  for (int *p = array_serial ; p < array_serial + SIZE; p += 2) {
    *p = 16;
  }

  for (int *p = array_serial+SIZE-1 ; p >= array_serial;  p -= 2) {
    *p = 9;
  }
  int *q = array_serial + SIZE-1;
  for (int *p = array+ SIZE-1; 
       p >= array  && q >= array_serial; p -= 1) {
      if (*p != *q)
	abort ();
      q -= 1;
  }

  return 0;
}
