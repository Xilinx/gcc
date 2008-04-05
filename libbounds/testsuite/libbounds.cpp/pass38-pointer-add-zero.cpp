int main()
{
  int* p;
  int i = 0;
  int a[10];

  /* a[10] = 0; */
  p = a;
  p = p + i;
  *p = 0;

  return 0;
}

