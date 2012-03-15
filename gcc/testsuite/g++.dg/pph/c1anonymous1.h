// {    xfail-if "ANONYMOUS MERGING" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "c0anonymous.h:4:16: error: 'anon_t' has a previous declaration here" "" { xfail *-*-* } 0 }

#ifndef	C1ANONYMOUS
#define	C1ANONYMOUS

#include "c0anonymous.h"

enum { first, second }; // { dg-bogus "'anon_t' referred to as enum" "" { xfail *-*-* } }

#endif
