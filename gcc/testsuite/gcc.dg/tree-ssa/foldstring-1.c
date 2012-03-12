/* { dg-do compile } */
/* { dg-options "-O1 -fdump-tree-forwprop1" } */

void
arf ()
{
  if (""[0] == 0)
    blah ();
}
/* { dg-final { scan-tree-dump-not "if \\(" "forwprop1"} } */
/* { dg-final { cleanup-tree-dump "forwprop1" } } */
