/* { dg-options "-ldl -lcilkrts" } */
/* { dg-do run } */

#if HAVE_IO
#include <cstdio>
#endif
#include <stdlib.h>

struct T {
  unsigned long val;
  T(unsigned long v) : val(v) {}
  ~T() {
  }
};

T cilk_fib(T t) {
  if (t.val < 2) return t;
  T t1 = _Cilk_spawn cilk_fib(t.val - 1);
  T t2 =             cilk_fib(t.val - 2);
 _Cilk_sync; 
  return t1.val + t2.val;
}

T sfib(T t) {
  if (t.val < 2) return t;
  T t1 = sfib(t.val - 1);
  T t2 = sfib(t.val - 2);
  return t1.val + t2.val;
}

int main(int argc, char **argv) {
  int n = 35;
  if (argc > 1)
    n = atoi(argv[1]);

#if HAVE_IO
  printf("  cilk_fib(%d) = %ld\n", n,   cilk_fib(n).val);
  printf("sfib(%d) = %ld\n", n, sfib(n).val);
#endif
  if (cilk_fib(n).val != sfib(n).val)
    abort ();
  return 0;
}
