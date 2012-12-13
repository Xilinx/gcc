#define ARRAY_SIZE 100000

int main(void)
{
  int ii = 0;
  int array[ARRAY_SIZE];
  
  _Cilk_for (ii = 0; ii < 10; ii <<= 2) /* { dg-error "Invalid loop increment operation." } */
    array[ii] = 5;

  _Cilk_for (ii = 0; ii < 10; ii *= 2) /* { dg-error "Invalid loop increment operation." } */
    array[ii] = 5;

  _Cilk_for (ii = 0; ii < 10; ii /= 2) /* { dg-error "Invalid loop increment operation." } */
    array[ii] = 5;

  _Cilk_for (ii = 0; ii < 10; ii = 5) /* { dg-error "Invalid loop increment operation." } */
    array[ii] = 5;

  _Cilk_for (ii = 0; ii < 10; ii >>= 2) /* { dg-error "Invalid loop increment operation." } */
    array[ii] = 5;
}
