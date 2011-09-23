 #include <stdio.h> 
#include <cilk/cilk.h>

#define SIZE 3

int main() {
  int *q; 
  int array[SIZE];

  for (int *p = array; p < array + SIZE; ++p) {
    *p = 2;
    printf("(p = %08x\t*p = %08x)\n", p, *p);
    fflush(stdout); 
  }
  fprintf(stdout, "cilk_for increment by 2\n");
  fflush(stdout);
  cilk_for (int *p = array ; p < array + SIZE; p += 2) {
    *p = 16;
    printf("(p = %08x\t*p = %08x)\n", p, *p);
    fflush(stdout);
  }

  fprintf(stdout, "cilk_for decrement by 2 (reading reverse)\n");
  fflush(stdout);
  cilk_for (int *p = array+SIZE-1 ; p >= array;  p -= 2) {
    *p = 9;
     printf("(p = %08x\t*p = %08x)\n", p, *p);
    fflush(stdout);
  }

  fprintf(stdout, "cilk_for reading reverse\n");
  fflush(stdout);
  for (int *p = array+ SIZE-1 ; p >= array ; p -= 1) {
    printf("(p = %08x\t*p = %08x\t)\n", p, *p);
    fflush(stdout);
  }

  puts ("done.");
  return 0;
}
