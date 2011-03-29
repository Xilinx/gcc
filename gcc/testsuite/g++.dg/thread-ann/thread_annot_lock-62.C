// Test the support for allowing non-const but non-modifying overloaded
// operator to be protected by reader locks.
// This is a good test case. (i.e. There should be no warning emitted by the
// compiler.)
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include <vector>
#include "thread_annot_common.h"

Mutex mu;

std::vector<int> counts GUARDED_BY(mu);

int foo(int key) {
  ReaderMutexLock l(&mu);
  return counts[key];
}
