/* { dg-do compile } */

static int
foo1 (int x, int y) 
{
  return x + y;
}

static int
foo2 (int x, int y) 
{
  return x - y;
}

int
__attribute__ ((ifunc))
foo (int x, int y)
{ 
  if ((x + y) == 34)
    return foo1;
  else
    return foo2;
}

/* { dg-error "parameter .\[x|y\]. used in indirect function .int foo.int, int.." ""  { target *-*-* } 17 } */
