/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O -fdump-tree-tmmemopt" } */

char c;

f()
{
  __tm_atomic {
    ++c;
  }
}

/* { dg-final { scan-tree-dump-times "RfW.*RU1 \\(&c\\);" 1 "tmmemopt" } } */
/* { dg-final { scan-tree-dump-times "WaW.*WU1 \\(&c," 1 "tmmemopt" } } */
