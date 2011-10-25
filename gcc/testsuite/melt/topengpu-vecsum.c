/* file gcc/testsuite/melt/topengpu-vecsum.c */

/* run in $GCCMELT_BUILD/gcc 
  ln -s $GCCMELT_SOURCE/gcc/testsuite/melt/topengpu-vecsum.c .
 ./cc1 -std=gnu99 -fmelt-mode=opengpu -fmelt-init=@melt-default-modules-quicklybuilt -fmelt-module-path=melt-modules:. -fmelt-source-path=melt-sources:. -fmelt-workdir=melt-workdir -fmelt-debug -O2 -fgraphite -floop-parallelize-all  topengpu-vecsum.c
  ## it could be useful to pass -fdump-tree-all -fdump-ipa-all

  ## it could be useful to pass -ftree-parallelize-loops=4 which
triggers autopar in tree-parloops.c */

void myvecsum (int sz, 
               /* NB: without the restrict below, the loop is not parallel for graphite */
               float* restrict tres,
               float* restrict tx,
               float* restrict ty)
{
  int ix;
  for (ix = 0; 
       ix < sz; 
       ix++)
    tres[ix] = 
      tx[ix] 
      + ty[ix];
}

/* eof topengpu-vecsum.c */
