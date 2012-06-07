/* { dg-do compile} */
/* { dg-options "-O2 -fprofile-generate -fprofile-generate-sampling" } */

void foobar(int);

void
foo (void)
{
  int i;
  for (i = 0; i < 100; i++)
    {
      foobar(i);
    }
}

void
bar (void)
{
  int i;
  for (i = 0; i < 100; i++)
    {
      foobar(i);
    }
}

/* { dg-final { cleanup-coverage-files } } */
