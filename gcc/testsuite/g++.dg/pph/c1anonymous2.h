// {    xfail-if "ANONYMOUS MERGING" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "c0anonymous.h:4:16: error: 'struct anon_t' has a previous declaration as 'struct anon_t'" "" { xfail *-*-* } 0 }

#ifndef C1ANONYMOUS2_H
#define C1ANONYMOUS2_H

#include "c0anonymous.h"

typedef struct { // { dg-bogus "conflicting declaration 'struct<anonymous>'" "" { xfail *-*-* } }

    char *field;
} anon2_t; // { dg-bogus "invalid type in declaration before ';' token" "" { xfail *-*-* } }

#endif
