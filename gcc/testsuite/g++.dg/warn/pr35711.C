// PR 35711
// { dg-do compile }
// { dg-options "-Wcast-qual" }

int* foo (volatile int *p)
{
  return (int*)p; // { dg-warning "cast from type 'int volatile\\*' to type 'int\\*' casts away qualifiers" }
}
