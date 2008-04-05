/* The class has space remaining at the end of a word (alignment) */

#include <iostream>
using std::cout;

#include <string.h>

class sTest {
public:
  int i;
  short a[3];
  char b[8];

  sTest(const char* str) { strcpy(b,str); }
};

int main()
{
    sTest s(". test\n");

    cout << s.b;

    return 0;
}
/* { dg-output "\.\ test\n" } */
