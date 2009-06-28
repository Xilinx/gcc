/* { dg-do compile } */

class ifunc
{
private:
  int foo1 (int);
  int foo2 (int);
  int i;

public:
  int foo (int);
};

int
__attribute__ ((ifunc))
ifunc::foo (int x)
{ 
  if ((x + i) == 34)
    return &ifunc::foo1;
  else
    return &ifunc::foo2;
}

/* { dg-error "parameter .this. used in indirect function .int ifunc::foo.int.." ""  { target *-*-* } 16 } */
/* { dg-error "parameter .x. used in indirect function .int ifunc::foo.int.." ""  { target *-*-* } 16 } */
