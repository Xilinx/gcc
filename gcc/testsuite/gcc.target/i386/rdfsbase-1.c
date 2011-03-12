/* { dg-do compile { target { x32 || lp64 } } } */
/* { dg-options "-O2 -mfsgsbase" } */
/* { dg-final { scan-assembler "rdfsbase\[ \t]+(%|)eax" } } */

#include <immintrin.h>

unsigned int
read_fs_base32 (void)
{
  return _readfsbase_u32 ();
}
