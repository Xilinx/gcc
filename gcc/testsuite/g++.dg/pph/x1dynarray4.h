#ifndef X1DYNARRAY4_H
#define X1DYNARRAY4_H

#include <stddef.h>

namespace tst {

template< typename T >
struct dynarray
{
    #include "a1dynarray-dcl1.hi"
    #include "a1dynarray-dcl2b.hi"
    #include "a1dynarray-dcl3.hi"
};

} // namespace tst

#endif
