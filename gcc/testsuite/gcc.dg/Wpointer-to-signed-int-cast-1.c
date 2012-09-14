/* Test -Wpointer-to-signed-int-cast - off by default.  */
/* { dg-do compile { target { int128 && ilp32 } } } */
/* { dg-options "-Wpointer-to-signed-int-cast" } */

void *p;

long
foo (void)
{
  return (long) p; /* { dg-warning "cast from pointer to signed integer" } */
}

unsigned long
bar (void)
{
  return (unsigned long) p;
}
