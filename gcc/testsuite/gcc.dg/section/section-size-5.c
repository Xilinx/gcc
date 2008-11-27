/* { dg-do link } */
/* { dg-options "-O2 -w -fpartition-functions-into-sections=300" } */
/* Same test as pr18628.c.  */

int i;

int main()
{
  for (;;)
  {
    switch (i)
    {
      case 0:
      case 1:
        return 1;

      case 2:
      case 3:
        return 0;

      case 5:
        --i;
    }
  }
}

