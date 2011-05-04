/* Check that __builtin_dispatch gets converted and the executable runs fine. 
   Since featureTest () returns 1, foo should be called and return 0.  Cloning
   and Hoisting is not turned on here. */
/* { dg-do run } */

int __attribute__ ((version_selector))
featureTest ()
{
  return 1;
}

int __attribute__ ((cold))
foo ()
{
  return 0;
}

int __attribute__ ((cold))
bar ()
{
  return 1;
}

int __attribute__ ((cold))
main ()
{
  int a, b;
  a = __builtin_dispatch (featureTest, (void *)foo, (void *) bar);
  b = __builtin_dispatch (featureTest, (void *)bar, (void *) foo);
  return a * b;
}
