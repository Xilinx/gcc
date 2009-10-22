/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O2" } */

extern long ringo(long int);
int g,i;

f()
{
  __transaction [[relaxed]] {
    for (i=0; i < 10; ++i)
      ringo(g);  
  }
}
