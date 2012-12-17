#define ARRAY_SIZE 1000

int a[ARRAY_SIZE];

int main(void)
{
  int i= 0;

  for (i = 0; i < ARRAY_SIZE; i++)
    a[i] = 0;
  
  _Cilk_for (i = (ARRAY_SIZE-1); i >= 0; i--) 
    a[i] = i;

  for (i = 0; i < ARRAY_SIZE; i++)
    if (a[i] != i)
      return 1;

  for (i = 0; i < ARRAY_SIZE; i++)
    a[i] = 0;
  
  _Cilk_for (i = (ARRAY_SIZE-1); i >= 0; i -= 1) 
    a[i] = i;

  for (i = 0; i < ARRAY_SIZE; i++)
    if (a[i] != i)
      return 1;

  for (i = 0; i < ARRAY_SIZE; i++)
    a[i] = 0;
  
  _Cilk_for (i = (ARRAY_SIZE-1); i >= 0; --i) 
    a[i] = i;

  for (i = 0; i < ARRAY_SIZE; i++)
    if (a[i] != i)
      return 1;

  return 0;
}
