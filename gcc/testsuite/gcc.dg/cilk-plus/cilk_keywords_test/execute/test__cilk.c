void abort (void);
void exit (int);
int main (void)
{
  if (__cilk == 200)
    exit (0);
  else
    abort ();
  return 0;
}
