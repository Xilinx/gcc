#include <cilk/cilk.h>
#include <iostream>
#include <cstdio>

using namespace std;
#define TEST 1


#define ITER 300

int n_errors;
#if TEST
void test (int *array, int n, int val) {
    for (int i = 0; i < n; ++i) {
        if (array[i] != val) {
            n_errors++;
            cout << "Value was wrong at " << i << "\t" ;
            cout << "found " << array[i] << " should be " << val << endl;
        }
    }
}
#endif
 

int main () {
    int array[ITER];

    cilk_for (int *j = (array); j < array + ITER; j += 1)  {
       *j = 6; 
//       printf("(p = %08x\t*p = %08x)\n", j, *j);
    }
#if TEST
    test(array, ITER, 6);
#endif

    cilk_for (int *i = array; i < array + ITER; i += 1) {
        *i = 1;
 //       printf("(p = %08x\t*p = %08x\t)\n", i, *i);
    }

#if TEST
    test(array, ITER, 1);
#endif

    cilk_for (int *i = array+ITER-1; i >= array; i -= 1) {
        *i = 8;
   //     printf("(p = %08x\t*p = %08x\t)\n", i, *i);
    }
#if TEST
    test(array, ITER, 8);
#endif
    cout << "Number of Errors = " << n_errors << endl;
    cout << "done." << endl;

    return 0;

}
