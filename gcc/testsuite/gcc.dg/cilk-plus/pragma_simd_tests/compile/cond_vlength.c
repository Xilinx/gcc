/* { dg-do run } */
/* { dg-options " -w " } */

int main ()
{
  int i;
  float a[256];

  // The line below should be OK! 
  #pragma simd vectorlength(sizeof (a) == sizeof (float) ? 4 : 8) 
  for (i = 0; i < 256; i++)
    {
      a[i] = i;
    }

  return 0;
}
