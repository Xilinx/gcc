
#include "x0tmplclass.h"

int wrapper<char>::cache = 2;

template
struct wrapper<short>;

template
long wrapper<long>::cache;

int main() {
  wrapper<char> vc;
  wrapper<short> vs;
  wrapper<int> vi;
  return 0;
}
