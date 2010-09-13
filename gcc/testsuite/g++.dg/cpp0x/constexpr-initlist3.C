// { dg-options -std=c++0x }

#include <initializer_list>

constexpr int f(std::initializer_list<int> l) { return l.begin()[0]; }

int main()
{
  constexpr int i = f({42});
  return (i != 42);
}
