/* { dg-do compile } */
/* { dg-options "-O2 -fpartition-functions-into-sections=150 -munsafe-dma -fdump-rtl-fpart" } */

#include <spu_intrinsics.h>
#include <spu_internals.h>
#define SPE_ADDR 0x0
#define EA_ADDR 0x2000

int
spu_dma (void)
{
  int status = 0;
  spu_writech (MFC_LSA, SPE_ADDR);
  spu_writech (MFC_EAH, 0x0);
  spu_writech (MFC_EAL, EA_ADDR);
  spu_writech (MFC_Size, 0x10);
  spu_writech (MFC_TagID, 5);
  spu_writech (MFC_Cmd, 0x20);
  return 0;
}

int
main ()
{
  spu_dma ();
}

/* { dg-final { scan-rtl-dump-times "starts a critical section" 1 "fpart" } } */
/* { dg-final { cleanup-rtl-dump "fpart" } } */


