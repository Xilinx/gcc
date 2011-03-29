// Test handling of additional (finer-grained) escape hatche attributes.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

Mutex *mu1, *mu2;

struct Foo {
  int a GUARDED_BY(mu1);
};

int x GUARDED_BY(mu1) = 1;
int y GUARDED_BY(mu2);

main() {
  int z;
  Foo w;
  ANNOTATE_IGNORE_READS_AND_WRITES_BEGIN();
  y = x + 1;
  x = y;
  ANNOTATE_IGNORE_READS_AND_WRITES_END();

  z = ANNOTATE_UNPROTECTED_READ(w.a) + 1;
  if (z > 1) {
    ANNOTATE_IGNORE_READS_BEGIN();
    z = x + 2;
  }
  else {
    ANNOTATE_IGNORE_READS_BEGIN();
    z = x + 1;
  }
  z = y;
}
