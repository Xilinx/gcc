// { dg-do compile }
// { dg-options "-Wreal-conversion" }

#include <stddef.h>

int
func1 (int a)
{
  double f = a;
  return f;			// { dg-warning "conversion to" }
}

double func3 ();

void
func2 ()
{
  double g = 3.2;
  float f;
  int t = g;			// { dg-warning "conversion to" }
  bool b = g;
  int p;
  p = f;			// { dg-warning "conversion to" }
  func1 (g);			// { dg-warning "conversion to" }
  char c = f;			// { dg-warning "conversion to" }
  size_t s;
  p = s;
  int q;
  q = func3 ();			// { dg-warning "conversion to" }
}
