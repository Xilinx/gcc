#ifndef A_H_
#define A_H_
#include <stdio.h>

class A
{
public:
  A() {
    x_ = 0;
    printf ("constructing\n");
  }

  void hello(void) {
    x_++;
    printf ("Hello World (%d)\n", x_);
  }

private:
  int x_;
};
#endif
