// { dg-xfail-if "UNIMPL TRAIT MANGLING" { "*-*-*" } { "-fpph-map=pph.map" } }
// { dg-bogus "bits/stl_uninitialized.h:178:12: sorry, unimplemented: mangling trait_expr" "" { xfail *-*-* } 0 }
// { dg-bogus "bits/allocator.h:153:12: sorry, unimplemented: mangling trait_expr" "" { xfail *-*-* } 0 }
// { dg-bogus "bits/stl_construct.h:98:12: sorry, unimplemented: mangling trait_expr" "" { xfail *-*-* } 0 }
// { dg-bogus "bits/stl_tempbuf.h:183:12: sorry, unimplemented: mangling trait_expr" "" { xfail *-*-* } 0 }
// { dg-bogus "bits/cpp_type_traits.h:87:12: internal compiler error: in write_template_arg_literal, at cp/mangle.c:2919" "" { xfail *-*-* } 0 }

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
