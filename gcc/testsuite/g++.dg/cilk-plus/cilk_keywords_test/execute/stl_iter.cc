/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */



#include <vector>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <cilk/cilk.h>
using namespace std;


int main(void)
{
vector <int> array;
vector <int> array_serial;

#if 1
for (int ii = -1; ii < 10; ii++)
{   
  array.push_back(ii);
  array_serial.push_back (ii);
}
#endif
cilk_for (vector<int>::iterator iter = array.begin(); iter != array.end();
          iter++)
{
   if (*iter  == 6) 
     *iter = 13;
}
for (vector<int>::iterator iter = array_serial.begin(); 
     iter != array_serial.end(); iter++)
{
   if (*iter  == 6) 
     *iter = 13;
}
sort (array.begin(), array.end());
sort (array_serial.begin(), array_serial.end());

vector <int>::iterator iter = array.begin ();
vector <int>::iterator iter_serial = array_serial.begin ();

while (iter != array.end () && iter_serial != array_serial.end ())
{
  if (*iter != *iter_serial)
    abort ();
  iter++;
  iter_serial++;
}

return 0;
}   


