// { dg-xfail-if "ICE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "c120060625-1.h:14:22: internal compiler error: invalid built-in macro .__FLT_MAX__." "" { xfail *-*-* } 0 }

#ifndef __PPH_GUARD_H
#define __PPH_GUARD_H

/* PR middle-end/28151 */
/* Testcase by Steven Bosscher <stevenb.gcc@gmail.com> */

_Complex float b;

void foo (void)
{
  _Complex float a = __FLT_MAX__;
  b = __FLT_MAX__ + a;
}
#endif

