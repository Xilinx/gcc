#ifndef X1FUNCTIONS_H
#define X1FUNCTIONS_H
#include "c1functions.h"
inline int extern_inline(int i)         // lazy body
{ return extern_then_body(i); }
struct type {
    int mbr_decl_only(int);
    int mbr_decl_then_def(int);
    inline int mbr_inl_then_def(int);
    int mbr_decl_inline(int i)          // lazy body
    { return mbr_decl_only( i ); }
    virtual int mbr_virtual_inline()    // lazy body, but circular dependence
    { return mbr_decl_only( 1 ); }
};
#endif
