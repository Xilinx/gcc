/* { dg-options "-Os -mno-xl-barrel-shift" } */

void testfunc(void)
{
    unsigned volatile int z = 8192;
    z >>= 8;
}
/* { dg-final { scan-assembler-not "\bsrli" } } */
/* { dg-final { scan-assembler "\ori\tr18,r0" } } */
/* { dg-final { scan-assembler "addk\tr(\[0-9]\|\[1-2]\[0-9]\|3\[0-1]),r(\[0-9]\|\[1-2]\[0-9]\|3\[0-1]),r0" } } */
/* { dg-final { scan-assembler "addik\tr18,r18,-1" } } */
/* { dg-final { scan-assembler "bneid\tr18,.-4" } } */
/* { dg-final { scan-assembler "\srl\tr(\[0-9]\|\[1-2]\[0-9]\|3\[0-1]),r(\[0-9]\|\[1-2]\[0-9]\|3\[0-1])" } } */
