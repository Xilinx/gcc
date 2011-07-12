#ifndef X0DYNARRAY4_H
#define X0DYNARRAY4_H

#include <stddef.h>

namespace tst {

template< typename T >
struct dynarray
{
    #include "a0dynarray-dcl1.hi"
    #include "a0dynarray-dcl2b.hi"
    #include "a0dynarray-dcl3.hi"
};

} // namespace tst

#endif
