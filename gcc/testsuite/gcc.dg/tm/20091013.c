/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O2" } */

extern long ringo(long int);
int g,i;

f()
{
  __tm_atomic {
    for (i=0; i < 10; ++i)
      ringo(g);  
  }
}
