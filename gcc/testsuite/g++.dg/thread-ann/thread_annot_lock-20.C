// Test if delayed binding works with static class members.
// This is a "good" test case that should not incur any thread safety warning.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

class Bar {
 public:
  static int func1() EXCLUSIVE_LOCKS_REQUIRED(mu1_);
  static int b_ GUARDED_BY(mu1_);
  static Mutex mu1_;
  static int a_ GUARDED_BY(mu1_);
};

Bar b1;

int Bar::func1()
{
  int res = 5;

  if (a_ == 4)
    res = b_;
  return res;
}
