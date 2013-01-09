/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */

#include <assert.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <vector>
#include <list>
#if HAVE_IO 
#include <stdio.h>
#endif
#define NUMBER 500000
#include <stdlib.h>
typedef std::pair<int, int> my_type_t;

long
valid_pairs(std::vector< my_type_t > my_list) 
{
  cilk_for (int ii = 0; ii < my_list.size(); ii++) 
    {
#if HAVE_IO
    fprintf(stderr, "my_list index: %d, size: %zu.\n", ii, my_list.size());
#endif
      if (ii < 0 || ii >= my_list.size())
	abort (); 
    }
  return 0;
}

int main(int argc, char **argv) 
{
  std::vector<my_type_t> my_list;

  for (int ii = 0; ii < NUMBER; ii++) 
    my_list.push_back(my_type_t(ii, ii));
  long res = valid_pairs(my_list);

  return 0;
}

