/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O" } */

extern __attribute__((transaction_safe)) void TMreleaseNode ();

int global;

__attribute__((transaction_safe))
TMrbtree_insert ()
{
  if (global)
    TMreleaseNode();
}
