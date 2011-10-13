/* FIXME pph - The following timeout may cause failures on slow targets.
   In general it takes no longer than a couple of seconds to compile
   this test, but the new merging code is having trouble with this.  */
/* { dg-timeout 15 } */
/* { dg-xfail-if "MERGE INFINITE LOOP" { *-*-* } { "-fpph-map=pph.map" } } */
#include "c0limits-externalid.h"
