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
cilk_for (vector<int>::iterator iter = array.begin(); iter != array.end();
          iter++)
{
   printf("%d\n", *iter);
   if (*iter  == 6) 
   {
     *iter = 13;
   }
}
cout << "Changed the number 6 to 13 " << endl;
cilk_for (vector<int>::iterator iter = array.begin(); iter != array.end();
          iter += 1)
{
   printf("%d\n", *iter);
   if (*iter == 7) 
     *iter = 15;
}

sort (array.begin(), array.end());

cout << "Changed the number 7 to 15, and array is sorted " << endl;
cilk_for (vector<int>::iterator iter3 = array.begin(); iter3 != array.end();
          iter3++)
{
   printf("%d\n", *iter3);
}

return 0;
}   


