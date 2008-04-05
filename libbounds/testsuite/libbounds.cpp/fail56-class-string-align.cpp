/* The class has space remaining at the end of a word (alignment) */

#include <iostream>
using std::cout;

#include <string.h>

class sTest {
public:
  int i;
  short a[3];
  char b[7];

  sTest(const char* str) { strcpy(b,str); }
};

int main()
{
    sTest s(". test\n");

    cout << s.b;

    return 0;
}

/* This testcase is supposed to fail, but it is not, because the
   checker is not strong enough.  */

/* { dg-output "bounds violation.*" { xfail *-*-* } } */
/* { dg-do run  } */
