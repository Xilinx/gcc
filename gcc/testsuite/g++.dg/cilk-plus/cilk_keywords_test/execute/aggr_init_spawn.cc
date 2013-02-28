/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */

class X {
  int val;
public:
  X(int v) : val(v) { }
  ~X() {}
};

X make_x(volatile int *q)
{
  while (!*q);
}


int main (void) 
{
  volatile int y = 0;
  const X &x = _Cilk_spawn make_x(&y); 
  y = 1;
  _Cilk_sync;
  return 0;
}
