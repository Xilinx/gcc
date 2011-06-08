// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "'mbr_decl_inline' was not declared in this scope" "" { xfail *-*-* } 0 }

#include "x1functions.h"

int type::mbr_decl_then_def(int i)      // need body
{ return mbr_decl_inline( i ); }

int type::mbr_inl_then_def(int i)       // lazy body
{ return mbr_decl_then_def( i ); }
