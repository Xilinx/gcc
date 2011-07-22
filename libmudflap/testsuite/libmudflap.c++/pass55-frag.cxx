/* { dg-options "-O2" }  -  This test is flaky under GRTE.  */
#include <vector>

int main() {
  std::vector<int> v;
  v.push_back(1);
  return 0;
} 
