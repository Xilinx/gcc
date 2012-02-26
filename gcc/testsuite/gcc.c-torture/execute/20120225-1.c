#include <assert.h>
int f(int a, int b) __attribute__((noinline,noclone));
int f(int a, int b)
{
  int c = a == 0;
  int d = b == 0;
  return c == 0 && d == 0;
}

int f1(int a, int b) __attribute__((noinline,noclone));
int f1(int a, int b) 
{
  int c = a!=0;
  int d = b !=0;
  return c&&d;
}


int main(void)
{
  assert (f(0,0) == f1(0,0));
  assert (f(0,1) == f1(0,1));
  assert (f(1,1) == f1(1,1));
  assert (f(1,0) == f1(1,0));
  return 0;
}
