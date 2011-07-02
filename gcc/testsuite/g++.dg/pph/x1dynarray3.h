#ifndef X1DYNARRAY3_H
#define X1DYNARRAY3_H

#include <stddef.h>
#include <new>

namespace tst {

template< typename T >
struct dynarray
{
    #include "a1dynarray-dcl1.hi"
    #include "a1dynarray-dcl2b.hi"
    #include "a1dynarray-dcl3.hi"
};

#include "a1dynarray-dfn1b.hi"
#include "a1dynarray-dfn2b.hi"
#include "a1dynarray-dfn3b.hi"

} // namespace tst

#endif
