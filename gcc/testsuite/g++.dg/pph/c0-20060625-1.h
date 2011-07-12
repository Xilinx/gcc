#ifndef C0_20060625_1_H
#define C0_20060625_1_H

/* PR middle-end/28151 */
/* Testcase by Steven Bosscher <stevenb.gcc@gmail.com> */

_Complex float b;

void foo (void)
{
  _Complex float a = __FLT_MAX__;
  b = __FLT_MAX__ + a;
}
#endif

