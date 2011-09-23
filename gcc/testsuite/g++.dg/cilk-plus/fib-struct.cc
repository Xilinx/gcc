#include <cilk/cilk.h>
#include <cstdio>
#include <pthread.h> 
#define FIB_PARAMETER 40
struct fib_struct
{
  int x;
  int *y;
  int z[3];
  struct fib_struct *ptr_next;
  struct fib_struct operator+(struct fib_struct &other) {
    struct fib_struct z ;
     z.x = (*this).x + (other.x);
    return z; 
  }
  struct fib_struct operator-(int other) {
    struct fib_struct z ;
    z.x = this->x - other;
    return z;
  }
};

struct fib_struct fib (struct fib_struct z) {
    if (z.x < 2) return z;
    struct fib_struct a = cilk_spawn fib(z - 1);
    struct fib_struct b = fib(z - 2);
    cilk_sync; 
    struct fib_struct c = a + b;
    return (a+b);
}


int sfib(int x)
{
  if (x < 2) return x;
  int a = sfib(x-1);
  int b = sfib(x-2);
  return (a+b);
}

int main () {
   struct fib_struct z ;
   z.x = FIB_PARAMETER; 
   
    std::printf("fib(%d) = %d\n", FIB_PARAMETER, fib(z).x);
    int parallel_fib = fib(z).x;
    int serial_fib = sfib(FIB_PARAMETER);
    if (serial_fib == parallel_fib)
      std::puts("correct.");
    else
      std::puts("error.");
    
    return 0;
}


