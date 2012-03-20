// { dg-xfail-if "identifier bindings not set properly" { "*-*-*" } { "-fpph-map=pph.map"} }
// { dg-bogus ".*Segmentation fault" "ICE trying to parse std::mbstate_t"  { xfail *-*-* } 0 }
#ifndef _X1_MBSTATE_H
#define _X1_MBSTATE_H
#include "x0mbstate_t.h"
// Name lookup for std::mbstate_t fails here.  Instead of returning the global
// type_decl for mbstate_t, it returns the "usings ::mbstate_t" declaration.
typedef std::mbstate_t state_type;
#endif
