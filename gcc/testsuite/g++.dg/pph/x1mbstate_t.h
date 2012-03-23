// { xfail-if "" { "*-*-*" } { "-fpph-map=pph.map" } }

#ifndef _X1_MBSTATE_H
#define _X1_MBSTATE_H
#include "x0mbstate_t.h"
// Name lookup for std::mbstate_t was failingfails here.  Instead of returning
// the global type_decl for mbstate_t, it was returning the
// "usings ::mbstate_t" declaration.
typedef std::mbstate_t state_type;  // { dg-bogus "'mbstate_t' in namespace 'std' does not name a type" "" { xfail *-*-* } }
#endif
