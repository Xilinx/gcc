/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */

#include <cilk/cilk.h>
#include <cstdlib>

#define TEST 1


#define ITER 300

int n_errors;
#if TEST
void test (int *array, int n, int val) {
    for (int i = 0; i < n; ++i) {
        if (array[i] != val) {
            abort ();
        }
    }
}
#endif
 

int main () {
    int array[ITER];

    cilk_for (int *j = (array); j < array + ITER; j += 1)  {
       *j = 6; 
    }
#if TEST
    test(array, ITER, 6);
#endif

    cilk_for (int *i = array; i < array + ITER; i += 1) {
        *i = 1;
    }

#if TEST
    test(array, ITER, 1);
#endif

    cilk_for (int *i = array+ITER-1; i >= array; i -= 1) {
        *i = 8;
    }
#if TEST
    test(array, ITER, 8);
#endif
  
    return 0;

}
