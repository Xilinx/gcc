// Test the fix for a bug introduced by the support of pass-by-reference
// paramters.
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

#include "thread_annot_common.h"

struct Foo { Foo & operator<< (bool) {} };
struct Bar { Foo & func () {} };
struct Bas { void operator& (Foo &) {} };
void mumble()
{
     Bas() & Bar().func() << "" << "";
     Bas() & Bar().func() << "";
}
