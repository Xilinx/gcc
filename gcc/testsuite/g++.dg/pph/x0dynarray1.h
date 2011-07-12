#ifndef X0DYNARRAY1_H
#define X0DYNARRAY1_H

extern "C" void *memcpy(void *dest, const void *src, unsigned long n);
extern "C" void exit(int) throw();

template< typename T >
struct dynarray
{
    #include "a0dynarray-dcl1.hi"
    #include "a0dynarray-dcl2a.hi"
    #include "a0dynarray-dcl3.hi"
};

#include "a0dynarray-dfn1a.hi"
#include "a0dynarray-dfn2a.hi"
#include "a0dynarray-dfn3a.hi"

#endif
