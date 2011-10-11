#ifndef X5DYNARRAY7_H
#define X5DYNARRAY7_H

#include <stddef.h>
#include <new>
#include <memory>
#include <stdexcept>
#include <iterator>

namespace tst {

template< typename T >
struct dynarray
{
    #include "a0dynarray-dcl1.hi"
    #include "a0dynarray-dcl2b.hi"
    #include "a0dynarray-dcl3.hi"
    #include "a0dynarray-dcl4.hi"
};

#include "a0dynarray-dfn1b.hi"
#include "a0dynarray-dfn2c.hi"
#include "a0dynarray-dfn3c.hi"

} // namespace tst

#endif
