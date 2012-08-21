#include <stdio.h>

extern "C" {
  void __cilkrts_foo();
  void __cilkrts_enter_frame (struct __cilkrts_stack_frame *sf);
  void __cilkrts_enter_frame (struct __cilkrts_stack_frame *sf) {
   return;
  }
}

void __cilkrts_foo()
{
   printf("Hello World.\n"); 
}

int main (void)
{
   __cilkrts_foo();

  return 0;
}


