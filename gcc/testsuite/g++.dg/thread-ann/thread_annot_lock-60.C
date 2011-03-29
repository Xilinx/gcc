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
  ANNOTATE_IGNORE_READS_BEGIN();
  y = x + 1;   // { dg-warning "Writing to variable 'y' requires lock" }
  x = y;       // { dg-warning "Writing to variable 'x' requires lock" }
  ANNOTATE_IGNORE_READS_END();
  ANNOTATE_IGNORE_WRITES_BEGIN();
  y = x + 1;   // { dg-warning "Reading variable 'x' requires lock" }
  x = y;       // { dg-warning "Reading variable 'y' requires lock" }
  ANNOTATE_IGNORE_WRITES_END();

  z = w.a + 1; // { dg-warning "Reading variable 'w.a' requires lock" }
  if (z > 1) {
    z = x + 2; // { dg-warning "Reading variable 'x' requires lock" }
  }
  else {
    ANNOTATE_IGNORE_READS_BEGIN();
    z = x + 1;
  }
  z = y;       // { dg-warning "Reading variable 'y' requires lock" }
}
