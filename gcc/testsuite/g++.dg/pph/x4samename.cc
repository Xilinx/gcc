// { dg-bogus "x4samename.cc:11:18: error: expected unqualified-id before '=' token" "" { xfail *-*-* } 0 }
// { dg-bogus "x4samename.cc:12:43: error: cannot convert 'const char.' to 'double' for argument '1' to 'int func.double.'" "" { xfail *-*-* } 0 }

#include "x0samename2.h"
#include "x0samename1.h"

extern struct doppelgaenger out;

int main() {
   int x = out.field;
   doppelgaenger = 3;
   return func(3.14) + func((const char*)0);
}
