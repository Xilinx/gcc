// {    xfail-if "INTRINSIC RETURN" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "atomicity.h:48:61: error: void value not ignored as it ought to be" "" { xfail *-*-* } 0 }

#ifndef X6DYNARRAY6_H
#define X6DYNARRAY6_H

#include "x0dynarray4.h"

#include <new>
#include <memory>
#include <stdexcept>

namespace tst {

#include "a0dynarray-dfn1b.hi"
#include "a0dynarray-dfn2c.hi"
#include "a0dynarray-dfn3c.hi"

} // namespace tst

#endif
