/* { dg-do compile } */
/* { dg-options "-O3 -fcilkplus" } */

struct stuff {
 char asdf;
 float f;
};

void add_floats(struct stuff *a, struct stuff *b, int n)
{
  int i;
#pragma simd assert
  for (i=0; i<n; i++) // { dg-error "loop not vectorized" }
    {
      a[i].f = a[i].f + b[i].f;
    }
}
