// { dg-bogus "Cannot open PPH file for reading" "" { xfail *-*-* } }
#define NAME v
#define VALUE 1
#include "d1symnotinc.h"
// { dg-excess-errors "compilation terminated" { xfail *-*-* } }
