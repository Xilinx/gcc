/* Check if parameters are passed correctly to the versioned function. */

/* { dg-do run } */

#include <stdio.h>
#include <assert.h>

int __attribute__ ((version_selector))
featureTest ()
{
  return 1;
}

int __attribute__ ((noinline))
foo (int n1, double n2, char n3)
{
  assert (n1 == 10);
  assert (n2 == 20.0);
  assert (n3 == 'c');
  return n1;
}


int __attribute__ ((noinline))
bar (int n1, double n2, char n3)
{
  assert (n1 == 10);
  assert (n2 == 20.0);
  assert (n3 == 'c');
  return (n1 - 10);
}

int main ()
{
  int a = __builtin_dispatch (featureTest, (void *)foo, (void *)bar,
                              10, 20.0, 'c');
  int b = __builtin_dispatch (featureTest, (void *)bar, (void *)foo,
                              10, 20.0, 'c');
  return a * b;
}
