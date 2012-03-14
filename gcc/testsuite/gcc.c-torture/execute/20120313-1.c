int f(int a, int b) __attribute__((noinline, noclone));
int f(int a, int b)
{
  int c = a| b;
  int d = a|b;
  return c^d;
}

int main(void)
{
  if (f(100, 200) != 0)
    __builtin_abort ();
  return 0;
}
