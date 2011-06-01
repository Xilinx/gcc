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
