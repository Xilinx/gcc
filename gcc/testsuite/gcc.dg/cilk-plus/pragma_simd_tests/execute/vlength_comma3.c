int a[100];

int
main (int argc, char **argv)
{
  int i;

  for (i = 0; i < 100; i++)
    if (argc == 1) 
      a[i] = 10;
    else
      a[i] = argc * 5;

#pragma simd vectorlength(4, sizeof (int) == 4 ? 4 : 8, 8)
  for (i = 0; i < 100; i++) 
    a[i] += 1;

  for (i = 0; i < 100; i++)
    if (a[i] != 11)
      return 1;

  return 0;
}

