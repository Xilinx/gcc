#include <cilk/cilk.h>

void f0(volatile int *steal_flag)
{
  /* Wait for steal_flag to be set (but don't wait forever) */
  for (int i = 0; !*steal_flag && i < 1000000000; ++i)
    ;
}

int f1()
{
  volatile int steal_flag = 0;
  cilk_spawn f0(&steal_flag);
  steal_flag = 1;  // Indicate stolen
  return 0;
}

void f2(int)
{
}

void f3()
{
  cilk_spawn f2(f1());
}

int main()
{
  f3();
  return 0;
}
