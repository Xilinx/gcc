#include <vector>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <cilk/cilk.h>
using namespace std;


int main(void)
{
vector <int> array;

#if 1
for (int ii = -1; ii < 10; ii++)
{   
  array.push_back(ii);
}
#endif
cout << "Printing the array backwards " << endl;
cilk_for (vector<int>::reverse_iterator iter4 = array.rbegin(); iter4 != array.rend();
          iter4++)
{
  printf("%d\n", *iter4);
  if (*iter4 == 0x8) {
    *iter4 = 9;
  }
}

cout << "Replaced all the 8 with 9 and reprinting the array backwards " << endl;
cilk_for (vector<int>::reverse_iterator iter2 = array.rbegin(); iter2 != array.rend();
          iter2 += 1) 
{
   printf("%d\n", *iter2);
   if ((*iter2 == 0x4) || (*iter2 == 0x7)) {
    *iter2 = 0x3;
   }
}
sort (array.begin(), array.end());

cout << "Replaced all 4, 7 with 3 and reprinting the sorted array backwards " 
     << endl;
cilk_for (vector<int>::reverse_iterator iter2 = array.rbegin(); iter2 != array.rend();
          ++iter2) 
{
   printf("%d\n", *iter2);
}

return 0;
}   


