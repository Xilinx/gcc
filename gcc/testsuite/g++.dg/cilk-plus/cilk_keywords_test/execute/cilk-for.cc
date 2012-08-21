/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */
#include <cstdlib>
extern void abort (void);
int main(int argc, char **argv)
{
  char Array1[26], Array2[26];
  char Array1_Serial[26], Array2_Serial[26];
  
  int ii = 0, error = 0;
  for (ii = 0; ii < 26; ii++)  
    { 
      Array1[ii] = 'A'+ii;
      Array1_Serial[ii] = 'A'+ii;
    }
  for (ii = 0; ii < 26; ii++)
    {
      Array2[ii] = 'a'+ii;
      Array2_Serial[ii] = 'a'+ii;
    }

  _Cilk_for (int iii = 0; iii < 26; iii++) 
    Array1[iii] = Array2[iii];

  for (ii = 0; ii < 26; ii++)
    Array1_Serial[ii] = Array2_Serial[ii];

  for (ii = 0; ii < 26; ii++)  {
    if (Array1_Serial[ii] != Array1[ii])  { 
	error = 1; 
    }
  }
  if (error)
    abort ();

  return 0;
}
