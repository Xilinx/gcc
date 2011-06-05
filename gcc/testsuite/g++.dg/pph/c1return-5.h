// { dg-options "-mpreferred-stack-boundary=4" }

#ifndef __PPH_GUARD_H
#define __PPH_GUARD_H

/* This compile only test is to detect an assertion failure in stack branch
   development.  */
struct bar
{
  int x;
} __attribute__((aligned(64)));


struct bar
foo (void) { }
#endif
