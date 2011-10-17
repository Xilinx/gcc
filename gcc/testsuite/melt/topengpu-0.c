/* file gcc/testsuite/melt/topengpu-0.c */

/* run in $GCCMELT_BUILD/gcc 
  ln -s $GCCMELT_SOURCE/gcc/testsuite/melt/topengpu-0.c .
 ./cc1 -std=gnu99 -fmelt-mode=opengpu -fmelt-init=@melt-default-modules-quicklybuilt -fmelt-module-path=melt-modules:. -fmelt-source-path=melt-sources:. -fmelt-workdir=melt-workdir -fmelt-debug -O2 -fgraphite -floop-parallelize-all  topengpu-0.c
  ## it could be useful to pass -fdump-tree-all -fdump-ipa-all
*/
float foobar (int sz, 
              /* we mean that all four arrays tr tx ty tz cannot be
                 aliases, and they each are sized by sz ; See
                 http://www.lysator.liu.se/c/restrict.html */
              float tr[static restrict sz], 
              float tx[static restrict sz], 
              float ty[static restrict sz],  
              float tz[static restrict sz])
{
  float r = 0.0;
  // I expect this loop to be parallel
  for (int i=0; i<sz; i++)
    {
      tr[i] = 
        tx[i] + 
        ty[i] * tz[i];
    }
  for (int i=0; i<sz; i++)
    {
      r += tr[i];
    }
  return r;
}
