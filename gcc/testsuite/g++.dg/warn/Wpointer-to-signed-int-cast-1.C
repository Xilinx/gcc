// Test -Wpointer-to-signed-int-cast - off by default.
// { dg-do compile { target { int128 && ilp32 } } }
// { dg-options "-Wpointer-to-signed-int-cast" }

void *p;

long
foo (void)
{
  return (long) p; // { dg-warning "cast from 'void\\*' to 'long int'" } 
}

unsigned long
bar (void)
{
  return (unsigned long) p;
}

long long
xxx (void)
{
  return (long long) p;
}
