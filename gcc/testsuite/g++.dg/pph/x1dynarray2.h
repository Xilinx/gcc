#ifndef X1DYNARRAY2_H
#define X1DYNARRAY2_H

extern "C" void *memcpy(void *dest, const void *src, unsigned long n);
extern "C" void exit(int) throw();

namespace tst {

template< typename T >
struct dynarray
{
    #include "a1dynarray-dcl1.hi"
    #include "a1dynarray-dcl2a.hi"
    #include "a1dynarray-dcl3.hi"
};

#include "a1dynarray-dfn1a.hi"
#include "a1dynarray-dfn2a.hi"
#include "a1dynarray-dfn3a.hi"

} // namespace tst

#endif
