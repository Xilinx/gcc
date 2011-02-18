#include "x1tmplfunc.h"
type val = 3;

template<>
int identity< type >(type arg)
{ return arg + val; }

template
short identity(short arg);

int main() {
  return identity( 'a' );
}
