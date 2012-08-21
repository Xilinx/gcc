
#include <cilk/cilk.h>
#include <cstdio>
#include <pthread.h> 
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
  bool operator<(int number) {
   return (this->x < number);
  }
    
};

template <typename T>
T fib (T z) {
    if (z < 2) return z;
    T a = cilk_spawn fib<T>(z - 1);
    T b = fib<T>(z - 2);
    cilk_sync; 
    T c = a + b;
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
     int z = 40;
     std::printf("fib(%d) = %d\n", 40, fib<int>(z));
     int parallel_fib = fib<int>(z);
     int serial_fib = sfib(40);
    if (serial_fib == parallel_fib)
    std::puts("correct.");
    else
    std::puts("error.");
    
    return 0;
}
