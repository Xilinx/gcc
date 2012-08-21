/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */

#include <cstdlib>

extern "C" {
  int __cilkrts_foo();
  void __cilkrts_enter_frame (struct __cilkrts_stack_frame *sf);
  void __cilkrts_enter_frame (struct __cilkrts_stack_frame *sf) {
   return;
  }
}

int __cilkrts_foo()
{
  return 5;

}

int main (void)
{
   if (__cilkrts_foo() != 5) 
     abort ();

  return 0;
}


