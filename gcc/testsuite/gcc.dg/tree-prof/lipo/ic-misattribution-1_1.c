/* { dg-options "-O2 -fdump-ipa-tree_profile_ipa" } */

extern void other_caller (void);

void
callee (void)
{
  return;
}

void
caller(void (*func) (void))
{
  func ();
}

/* { dg-final-use { scan-ipa-dump "Indiret call -> direct call"  "tree_profile_ipa" } } */
/* { dg-final-use { cleanup-ipa-dump "tree_profile_ipa" } } */
