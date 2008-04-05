/* From mudflap bug #19319 */
#include <stack>

int main() {
  std::stack<int> v;
} 

