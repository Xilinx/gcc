/* { dg-timeout 15 } */
/* { dg-xfail-if "BOGUS MERGE HUGE SYMBOL LIST" { *-*-* } { "-fpph-map=pph.map" } } */
/* FIXME pph - The following timeout may cause failures on slow targets.
   In general it takes no longer than a couple of seconds to compile
   this test, but the new merging code is having trouble with this.
   Probably due to an O(n^2) merging algorithm.  */

#include "c0limits-externalid.h"
