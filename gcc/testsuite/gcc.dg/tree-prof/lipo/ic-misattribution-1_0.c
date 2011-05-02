/* { dg-options "-O2 -fdump-ipa-tree_profile_ipa" } */

extern void callee (void);
extern void caller (void (*func) (void));

typedef void (*func_t) (void);
func_t func;

int
main ()
{
  func = callee;
  caller (callee);
  func ();
  return 0;
}

/* { dg-final-use { scan-ipa-dump-times "Indirect call -> direct call" 2 "tree_profile_ipa" } } */
/* { dg-final-use { cleanup-ipa-dump "tree_profile_ipa" } } */
