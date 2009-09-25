/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O -fdump-tree-tmmemopt" } */

long g, xxx, yyy;
extern george() __attribute__((tm_callable));
extern ringo(long int);
int i;

f()
{
  __tm_atomic {
    g = 666;
    george();
    if (i == 9)
      goto bye;
    xxx=8;
    yyy=9;
    for (i=0; i < 10; ++i)
      ringo(g);  
  bye:
    ringo(g);
  }
}

/* { dg-final { scan-tree-dump-times "RaW.*RU8 \\(&g\\);" 1 "tmmemopt" } } */
/* { dg-final { scan-tree-dump-times "WaR.*WU4 \\(&i," 1 "tmmemopt" } } */
/* { dg-final { scan-tree-dump-times "RaW.*RU4 \\(&i\\);" 1 "tmmemopt" } } */
/* { dg-final { scan-tree-dump-times "WaW.*WU4 \\(&i," 1 "tmmemopt" } } */
