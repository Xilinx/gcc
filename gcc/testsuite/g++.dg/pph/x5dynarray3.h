#ifndef X5DYNARRAY3_H
#define X5DYNARRAY3_H

#include <stddef.h>
#include <new>

namespace tst {

template< typename T >
struct dynarray
{
    #include "a0dynarray-dcl1.hi"
    #include "a0dynarray-dcl2b.hi"
    #include "a0dynarray-dcl3.hi"
};

#include "a0dynarray-dfn1b.hi"
#include "a0dynarray-dfn2b.hi"
#include "a0dynarray-dfn3b.hi"

} // namespace tst

#endif
