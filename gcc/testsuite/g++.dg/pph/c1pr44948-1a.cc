// { dg-xfail-if "INFINITE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "internal compiler error: in lto_get_pickled_tree, at lto-streamer-in.c" "" { xfail *-*-* } 0 }
#include "c0pr44948-1a.h"
