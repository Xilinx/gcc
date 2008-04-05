/* From mudflap bug #19319 */
#include <vector>

int main() {
  std::vector<int> v;
  v.push_back(1);

  return 0;
} 

