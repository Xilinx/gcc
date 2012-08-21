/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */

#include <cstdlib>
int main(int argc, char **argv)
{
  int x = 0;
  if (argc == 1) 
    goto bye;
  x = 5;
bye:
  if ((argc == 1) && (x != 0))
    abort ();
  return 0;
}
