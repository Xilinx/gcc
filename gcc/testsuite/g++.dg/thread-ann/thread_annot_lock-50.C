// Test the support for allowing non-const but non-modifying methods to be
// protected by reader locks.
// This is a good test case. (i.e. There should be no warning emitted by the
// compiler.)
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include <map>
#include "thread_annot_common.h"

typedef std::map<int, int> MyMapType;

Mutex mu;
MyMapType MyMap GUARDED_BY(mu);

int foo(int key) {
  ReaderMutexLock l(&mu);
  MyMapType::const_iterator iter = MyMap.find(key);
  if (iter != MyMap.end()) {
    return iter->second;
  }
}
