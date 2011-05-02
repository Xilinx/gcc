/* { dg-options "-O2 -fdump-tree-optimized -fdump-ipa-tree_profile_ipa" } */

extern void setp (int (**pp) (void), int i);

int
main (void)
{
  int (*p) (void);
  int  i;

  for (i = 0; i < 10; i ++)
    {
	setp (&p, i);
	p ();
    }
  
  return 0;
}

/* { dg-final-use { scan-ipa-dump "Indirect call -> direct call.* a1" "tree_profile_ipa"} } */
/* { dg-final-use { scan-tree-dump-not "Invalid sum" "optimized"} } */
/* { dg-final-use { cleanup-tree-dump "optimized" } } */
/* { dg-final-use { cleanup-ipa-dump "tree_profile_ipa" } } */
