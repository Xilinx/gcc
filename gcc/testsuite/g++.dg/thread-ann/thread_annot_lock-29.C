// Test the handling of lock expressions that contain non-const array indices.
// This is a "good" test that should not incur any compilation wanrings.
// { dg-do compile }
// { dg-options "-Wthread-safety -Wthread-unsupported-lock-name -O" }

#include "thread_annot_common.h"

struct Foo {
  Mutex mu;
  int data;
};

const int MaxArraySize = 16;

Foo foo[MaxArraySize];

int func() {
  int result = 0;
  for (int cpu = 0; cpu != MaxArraySize; cpu++) {
    foo[cpu].mu.Lock();
    result += foo[cpu].data;
    foo[cpu].mu.Unlock();
  }
  return result;
}
