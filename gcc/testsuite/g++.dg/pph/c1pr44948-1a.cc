// { dg-xfail-if "INFINITE" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "internal compiler error: in lto_streamer_cache_get, at lto-streamer.c" "" { xfail *-*-* } 0 }
#include "c0pr44948-1a.h"
