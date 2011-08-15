/*  reducer.h                  -*-C++-*-
 *
 * Copyright (C) 2009-2011 
 * Intel Corporation
 * 
 * This file is part of the Intel Cilk Plus Library.  This library is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * Under Section 7 of GPL version 3, you are granted additional
 * permissions described in the GCC Runtime Library Exception, version
 * 3.1, as published by the Free Software Foundation.
 * 
 * You should have received a copy of the GNU General Public License and
 * a copy of the GCC Runtime Library Exception along with this program;
 * see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CILK_REDUCER_H_INCLUDED
#define CILK_REDUCER_H_INCLUDED

#include "cilk/hyperobject_base.h"

/*
 * C++ and C interfaces for Cilk reducer hyperobjects
 */

/* Utility macros */
#define __CILKRTS_MKIDENT(a,b) __CILKRTS_MKIDENT_IMP(a,b,)
#define __CILKRTS_MKIDENT3(a,b,c) __CILKRTS_MKIDENT_IMP(a,b,c)
#define __CILKRTS_MKIDENT_IMP(a,b,c) a ## b ## c

#ifdef __cplusplus

//===================== C++ interfaces ===================================

#include <new>

#ifdef CILK_STUB
// Stub implementations are in the cilk::stub namespace
namespace cilk {
    namespace stub { }
    using namespace stub;
}
#endif

// MONOID CONCEPT AND monoid_base CLASS TEMPLATE
//
// In mathematics, a "monoid" comprises a set of values (type), an associative
// operation on that set, and an identity value for that set and that
// operation.  So for example (integer, +, 0) is a monoid, as is (real, *, 1).
// The 'Monoid' concept in Cilk++ has a typedef and three functions that
// represent a that map to a monoid, (T, OP, IDENTITY), as follows:
//..
//  value_type          is a typedef for T
//  reduce(left,right)  evaluates '*left = *left OP *right'
//  identity(p)         constructs IDENTITY value into the uninitilized '*p'
//  destroy(p)          calls the destructor on the object pointed-to by 'p'
//  allocate(size)      return a pointer to size bytes of raw memory
//  deallocate(p)       deallocate the raw memory at p
//..
// 'left', 'right', and 'p' are all pointers to objects of type 'value_type'.
// All functions must be either 'static' or 'const'.  A class that meets the
// requirements of the 'Monoid' concept is usually stateless, but will
// sometimes contain state used to initialize the identity object.

namespace cilk {

/// The 'monoid_base' class template is a useful base class for a large set
/// of monoid classes for which the identity value is a default-constructed
/// value of type 'T', allocated using operator new.  A derived class of
/// 'monoid_base' need only declare and implement the 'reduce' function.
template <class T>
class monoid_base
{
public:
    /// Type of value for this monoid
    typedef T value_type;

    /// Constructs IDENTITY value into the uninitilized '*p'
    void identity(T* p) const { new ((void*) p) T(); }

    /// Calls the destructor on the object pointed-to by 'p'
    void destroy(T* p) const { p->~T(); }

    /// Return a pointer to size bytes of raw memory
    void* allocate(size_t s) const { return operator new(s); }

    /// Deallocate the raw memory at p
    void deallocate(void* p) const { operator delete(p); }
};

} // end namspace cilk

#ifndef CILK_STUB

namespace cilk {

/// reducer CLASS TEMPLATE
///
/// A reducer is instantiated on a Monoid.  The Monoid provides the value type,
/// associative reduce function, and identity for the reducer.  Function view()
/// and operator()() return the current view of the reducer.
template <class Monoid>
class reducer
{
    typedef typename Monoid::value_type value_type;

    __cilkrts_hyperobject_base  base_;
    const Monoid                monoid_; // implementation of monoid interface
    void*                       initialThis_; // Sanity checker

    __CILKRTS_CACHE_ALIGNED(value_type leftmost_);

    // Wrappers around C monoid dispatch functions
    static void reduce_wrapper(void* r, void* lhs, void* rhs);
    static void identity_wrapper(void* r, void* view);
    static void destroy_wrapper(void* r, void* view);
    static void* allocate_wrapper(void* r, __STDNS size_t bytes);
    static void deallocate_wrapper(void* r, void* view);

    void init();

    /* disable copy */
    reducer(const reducer&);
    reducer& operator=(const reducer&);

  public:
    reducer() : monoid_(), leftmost_()
    {
        init();
    }

    /// Special case: allow reducer(A) construction from both const and
    /// non-const reference to A.  Allowing this for all argument combinations
    /// is desirable but would result in at least 93 overloads.
    template <typename A>
    explicit reducer(A& a)
        : base_(), monoid_(), leftmost_(a)
    {
        init();
    }

    template <typename A>
    explicit reducer(const A& a)
      : base_(), monoid_(), leftmost_(a)
    {
        init();
    }

    template <typename A, typename B>
    reducer(const A& a, const B& b)
        : base_(), monoid_(), leftmost_(a,b)
    {
        init();
    }

    template <typename A, typename B, typename C>
    reducer(const A& a, const B& b, const C& c)
      : base_(), monoid_(), leftmost_(a,b,c)
    {
        init();
    }

    template <typename A, typename B, typename C, typename D>
    reducer(const A& a, const B& b, const C& c, const D& d)
      : base_(), monoid_(), leftmost_(a,b,c,d)
    {
        init();
    }

    template <typename A, typename B, typename C, typename D, typename E>
    reducer(const A& a, const B& b, const C& c, const D& d, const E& e)
      : base_(), monoid_(), leftmost_(a,b,c,d,e)
    {
        init();
    }

    // Special case: both const and non-const Monoid reference are needed
    // so that reducer(Monoid&) is more specialised than
    // template <typename A> explicit reducer(A& a) and
    // reducer(const Monoid&) is more specialised than
    // template <typename A> explicit reducer(const A& a)
    explicit reducer(Monoid& hmod)
        : base_(), monoid_(hmod), leftmost_()
    {
        init();
    }

    explicit reducer(const Monoid& hmod)
        : base_(), monoid_(hmod), leftmost_()
    {
        init();
    }

    // Special case: allow reducer(Monoid,A) construction from both const and
    // non-const references to A.  Allowing this for all argument combinations
    // is desirable but would result in at least 93 overloads.
    template <typename A>
    reducer(const Monoid& hmod, A& a)
      : base_(), monoid_(hmod), leftmost_(a)
    {
        init();
    }

    template <typename A>
    reducer(const Monoid& hmod, const A& a)
      : base_(), monoid_(hmod), leftmost_(a)
    {
        init();
    }

    template <typename A, typename B>
    reducer(const Monoid& hmod, const A& a, const B& b)
      : base_(), monoid_(hmod), leftmost_(a,b)
    {
        init();
    }

    template <typename A, typename B, typename C>
    reducer(const Monoid& hmod, const A& a, const B& b, const C& c)
      : base_(), monoid_(hmod), leftmost_(a,b,c)
    {
        init();
    }

    template <typename A, typename B, typename C, typename D>
    reducer(const Monoid& hmod, const A& a, const B& b, const C& c,
                const D& d)
      : base_(), monoid_(hmod), leftmost_(a,b,c,d)
    {
        init();
    }

    template <typename A, typename B, typename C, typename D, typename E>
    reducer(const Monoid& hmod, const A& a, const B& b, const C& c,
                const D& d, const E& e)
      : base_(), monoid_(hmod), leftmost_(a,b,c,d,e)
    {
        init();
    }

    __CILKRTS_STRAND_STALE(~reducer());

    /* access the unwrapped object */
    value_type& view() {
        /* look up reducer in current map */
        return *static_cast<value_type *>(__cilkrts_hyper_lookup(&base_));
    }

    value_type const& view() const {
        /* look up reducer in current map */
        return const_cast<reducer*>(this)->view();
    }

    value_type&       operator()()       { return view(); }
    value_type const& operator()() const { return view(); }

    const Monoid& monoid() const { return monoid_; }
};

template <typename Monoid>
void reducer<Monoid>::init()
{
    static const cilk_c_monoid c_monoid_initializer = {
        (cilk_c_reducer_reduce_fn_t)     &reduce_wrapper,
        (cilk_c_reducer_identity_fn_t)   &identity_wrapper,
        (cilk_c_reducer_destroy_fn_t)    &destroy_wrapper,
        (cilk_c_reducer_allocate_fn_t)   &allocate_wrapper,
        (cilk_c_reducer_deallocate_fn_t) &deallocate_wrapper
    };

    base_.__c_monoid = c_monoid_initializer;
    base_.__flags = 0;
    base_.__view_offset = (char*) &leftmost_ - (char*) this;
    base_.__view_size = sizeof(value_type);
    initialThis_ = this;

    __cilkrts_hyper_create(&base_);
}

template <typename Monoid>
void reducer<Monoid>::reduce_wrapper(void* r, void* lhs, void* rhs)
{
    reducer* self = static_cast<reducer*>(r);
    self->monoid_.reduce(static_cast<value_type*>(lhs),
                         static_cast<value_type*>(rhs));
}

template <typename Monoid>
void reducer<Monoid>::identity_wrapper(void* r, void* view)
{
    reducer* self = static_cast<reducer*>(r);
    self->monoid_.identity(static_cast<value_type*>(view));
}

template <typename Monoid>
void reducer<Monoid>::destroy_wrapper(void* r, void* view)
{
    reducer* self = static_cast<reducer*>(r);
    self->monoid_.destroy(static_cast<value_type*>(view));
}

template <typename Monoid>
void* reducer<Monoid>::allocate_wrapper(void* r, __STDNS size_t bytes)
{
    reducer* self = static_cast<reducer*>(r);
    return self->monoid_.allocate(bytes);
}

template <typename Monoid>
void reducer<Monoid>::deallocate_wrapper(void* r, void* view)
{
    reducer* self = static_cast<reducer*>(r);
    self->monoid_.deallocate(static_cast<value_type*>(view));
}

template <typename Monoid>
__CILKRTS_STRAND_STALE(reducer<Monoid>::~reducer())
{
    // Make sure we haven't been memcopy'd or corrupted
    __CILKRTS_ASSERT(this == initialThis_);
    __cilkrts_hyper_destroy(&base_);
}

} // end namespace cilk

#else // if defined(CILK_STUB)

/**************************************************************************
 *        Stub reducer implementation
 **************************************************************************/

namespace cilk {
namespace stub {

template <class Monoid>
class reducer {
    typedef typename Monoid::value_type value_type;

    const Monoid monoid_;
    value_type   obj_;

    /* disable copy */
    reducer(const reducer&);
    reducer& operator=(const reducer&);

  public:
    reducer() : monoid_(), obj_() { }

    // Special case: allow reducer(A) construction from both const and
    // non-const reference to A.  Allowing this for all argument combinations
    // is desirable but would result in at least 93 overloads.
    template <typename A>
    explicit reducer(A& a)
      : monoid_(), obj_(a) {
    }

    template <typename A>
    explicit reducer(const A& a)
      : monoid_(), obj_(a) {
    }

    template <typename A, typename B>
    reducer(const A& a, const B& b)
      : monoid_(), obj_(a, b) {
    }

    template <typename A, typename B, typename C>
    reducer(const A& a, const B& b, const C& c)
      : monoid_(), obj_(a, b, c)
    {
    }

    template <typename A, typename B, typename C, typename D>
    reducer(const A& a, const B& b, const C& c, const D& d)
      : monoid_(), obj_(a, b, c, d)
    {
    }

    template <typename A, typename B, typename C, typename D, typename E>
    reducer(const A& a, const B& b, const C& c, const D& d, const E& e)
      : monoid_(), obj_(a, b, c, d, e)
    {
    }

    // Special case: both const and non-const Monoid reference are needed
    // so that reducer(Monoid&) is more specialised than
    // template <typename A> explicit reducer(A& a) and
    // reducer(const Monoid&) is more specialised than
    // template <typename A> explicit reducer(const A& a)
    explicit reducer(Monoid& m) : monoid_(m), obj_() { }
    explicit reducer(const Monoid& m) : monoid_(m), obj_() { }

    // Special case: allow reducer(Monoid,A) construction from both const and
    // non-const references to A.  Allowing this for all argument combinations
    // is desirable but would result in at least 93 overloads.
    template <typename A>
    reducer(const Monoid& m, A& a)
      : monoid_(m), obj_(a) {
    }

    template <typename A>
    reducer(const Monoid& m, const A& a)
      : monoid_(m), obj_(a) {
    }

    template <typename A, typename B>
    reducer(const Monoid& m, const A& a, const B& b)
      : monoid_(m), obj_(a, b) {
    }

    template <typename A, typename B, typename C>
    reducer(const Monoid& m, const A& a, const B& b, const C& c)
      : monoid_(m), obj_(a, b, c)
    {
    }

    template <typename A, typename B, typename C, typename D>
    reducer(const Monoid& m, const A& a, const B& b, const C& c, const D& d)
      : monoid_(m), obj_(a, b, c, d)
    {
    }

    template <typename A, typename B, typename C, typename D, typename E>
    reducer(const Monoid& m, const A& a, const B& b, const C& c,
                const D& d, const E& e)
      : monoid_(m), obj_(a, b, c, d, e)
    {
    }

    ~reducer() { }

    value_type&       view()       { return obj_; }
    value_type const& view() const { return obj_; }

    value_type&       operator()()       { return view(); }
    value_type const& operator()() const { return view(); }

    const Monoid& monoid() const { return monoid_; }

}; // stub::reducer

} // end namespace stub
} // end namespace cilk

#endif // CILK_STUB

#endif /* __cplusplus */

/*===================== C interfaces ===================================*/

#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
# define _Typeof __typeof__
#endif

/* MACROS FOR DEFINING AND USING C REDUCERS
 *
 * Example use of these macros
 *
 *  double array[ARRAY_LEN];
 *  double sum()
 *  {
 *      extern void* double_summing_identity();
 *      extern void double_summing_reduce(void* lhs, void* rhs);
 *
 *      CILK_C_DECLARE_REDUCER(double) total =
 *          CILK_C_INIT_REDUCER(sizeof(double),
 *                              double_summing_reduce,
 *                              double_summing_identity,
 *                              free,
 *                              0);
 *      int i;
 *
 *      CILK_C_REGISTER_REDUCER(total);
 *
 *      cilk_for (i = 0; i < ARRAY_LEN; ++i)
 *          REDUCER_VIEW(total) += array[i];
 *
 *      CILK_C_UNREGISTER_REDUCER(total);
 *
 *      return total.value;
 *  }
 */

/***************************************************************************
 *              Common to real and stub implementations
 ***************************************************************************/

__CILKRTS_BEGIN_EXTERN_C

#define __CILKRTS_DECLARE_REDUCER_IDENTITY(name,tn)  CILK_EXPORT         \
    void __CILKRTS_MKIDENT3(name,_identity_,tn)(void* key, void* v)
#define __CILKRTS_DECLARE_REDUCER_REDUCE(name,tn,l,r) CILK_EXPORT        \
    void __CILKRTS_MKIDENT3(name,_reduce_,tn)(void* key, void* l, void* r)
#define __CILKRTS_DECLARE_REDUCER_DESTROY(name,tn,p) CILK_EXPORT         \
    void __CILKRTS_MKIDENT3(name,_destroy_,tn)(void* key, void* p)

__CILKRTS_END_EXTERN_C


#ifndef CILK_STUB

/***************************************************************************
 *              Real implementation
 ***************************************************************************/

__CILKRTS_BEGIN_EXTERN_C

/* Declare a reducer with 'Type' value type */
#define CILK_C_DECLARE_REDUCER(Type) struct {                      \
        __cilkrts_hyperobject_base   __cilkrts_hyperbase;          \
        __CILKRTS_CACHE_ALIGNED(Type value);                       \
    }

/* Initialize a reducer using the Identity, Reduce, and Destroy functions
 * (the monoid) and with an arbitrary-length comma-separated initializer list.
 */
#define CILK_C_INIT_REDUCER(T,Reduce,Identity,Destroy, ...)                  \
    { { { Reduce,Identity,Destroy,                                           \
          __cilkrts_hyperobject_alloc,__cilkrts_hyperobject_dealloc },       \
                0, __CILKRTS_CACHE_LINE__, sizeof(T) }, __VA_ARGS__ }

/* Register a local reducer. */
#define CILK_C_REGISTER_REDUCER(Expr) \
    __cilkrts_hyper_create(&(Expr).__cilkrts_hyperbase)

/* Unregister a local reducer. */
#define CILK_C_UNREGISTER_REDUCER(Expr) \
    __cilkrts_hyper_destroy(&(Expr).__cilkrts_hyperbase)

/* Get the current view for a reducer */
#define REDUCER_VIEW(Expr) (*(_Typeof((Expr).value)*)               \
    __cilkrts_hyper_lookup(&(Expr).__cilkrts_hyperbase))

__CILKRTS_END_EXTERN_C

#else /* if defined(CILK_STUB) */

/***************************************************************************
 *              Stub implementation
 ***************************************************************************/

__CILKRTS_BEGIN_EXTERN_C

/* Declare a reducer with 'Type' value type */
#define CILK_C_DECLARE_REDUCER(Type) struct {                      \
        Type                    value;                             \
    }

/* Initialize a reducer using the Identity, Reduce, and Destroy functions
 * (the monoid) and with an arbitrary-length comma-separated initializer list.
 */
#define CILK_C_INIT_REDUCER(T,Identity,Reduce,Destroy, ...)        \
    { __VA_ARGS__ }

/* Register a local reducer. */
#define CILK_C_REGISTER_REDUCER(Expr) ((void) Expr)

/* Unregister a local reducer. */
#define CILK_C_UNREGISTER_REDUCER(Expr) ((void) Expr)

/* Get the current view for a reducer */
#define REDUCER_VIEW(Expr) ((Expr).value)

__CILKRTS_END_EXTERN_C

#endif /* CILK_STUB */

#endif // CILK_REDUCER_H_INCLUDED
