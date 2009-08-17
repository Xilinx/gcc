/* { dg-do compile { target microblaze*-*-* } } */

/* { dg-options "" } */

#include <stdio.h>
#include <string.h>


/* { dg-final { scan-assembler "\.rodata*" } } */
const int global = 10;

int testfunc ()
{
    return global;
}
