// { dg-xfail-if "DEFAULT TEMPLATE ARG MERGING" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "Trying to merge distinct trees from the same PPH image x0tmpldfltparm.pph" "" { xfail *-*-* } 0 }

#include "x0tmpldfltparm.h"

handlei x;
handlel y;
