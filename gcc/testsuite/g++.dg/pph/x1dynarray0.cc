// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }

#include "x1dynarray1.h"

int main() {
    dynarray< int > alpha(8);
    return 0;
}
