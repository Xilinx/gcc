/* { dg-do compile } */
/* { dg-options "-O2" } */

int __attribute__ ((version_selector))
featureTest ()
{
  return 1;
}
