// { dg-xfail-if "BOGUS" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "a0dynarray-dfn1b.hi:3:19: error: there are no arguments to 'alloc' that depend on a template parameter, so a declaration of 'alloc' must be available" "" { xfail *-*-* } 0 }
// { dg-bogus "a0dynarray-dfn3b.hi:2:36: error: no 'void tst::dynarray<T>::check.tst::dynarray<T>::size_type.' member function declared in class 'tst::dynarray<T>'" "" { xfail *-*-* } 0 }

#ifndef X6DYNARRAY5_H
#define X6DYNARRAY5_H

#include "x0dynarray4.h"

#include <new>
#include <memory>

namespace tst {

#include "a0dynarray-dfn1b.hi"
#include "a0dynarray-dfn2c.hi"
#include "a0dynarray-dfn3b.hi"

} // namespace tst

#endif
