#ifndef X1DYNARRAY1_H
#define X1DYNARRAY1_H

#include <memory>

#define DefaultConstructible typename
#define CPP0X( ignore )

namespace std {

template< DefaultConstructible T >
struct dynarray
{
    // types:
    typedef       T                               value_type;
    typedef       T&                              reference;
    typedef const T&                              const_reference;
    typedef       T*                              iterator;
    typedef const T*                              const_iterator;
    typedef unsigned long                         size_type;
    typedef long                                  difference_type;

    // fields:
private:
    T*        store;
    size_type count;

    // helper functions:
    void check(size_type n)
        { if ( n >= count ) throw n; }
    T* alloc(size_type n)
        { return reinterpret_cast<T*>( new char[ n*sizeof(T) ] ); }

public:
    // construct and destruct:
    dynarray() CPP0X( = delete ) ;
    const dynarray operator=(const dynarray&) CPP0X( = delete ) ;

    explicit dynarray(size_type c)
        : store( alloc( c ) ), count( c )
        { size_type i;
          try {
              for ( size_type i = 0; i < count; ++i )
                  new (store+i) T;
          } catch ( ... ) {
              for ( ; i > 0; --i )
                 (store+(i-1))->~T();
              throw;
          } }

    dynarray(const dynarray& d)
        : store( alloc( d.count ) ), count( d.count )
        { try { uninitialized_copy( d.begin(), d.end(), begin() ); }
          catch ( ... ) { delete store; throw; } }

    ~dynarray()
        { for ( size_type i = 0; i < count; ++i )
              (store+i)->~T();
          delete[] store; }

    // iterators:
    iterator       begin()        { return store; }
    const_iterator begin()  const { return store; }
    const_iterator cbegin() const { return store; }
    iterator       end()          { return store + count; }
    const_iterator end()    const { return store + count; }
    const_iterator cend()   const { return store + count; }

    // capacity:
    size_type size()     const { return count; }
    size_type max_size() const { return count; }
    bool      empty()    const { return false; }

    // element access:
    reference       operator[](size_type n)       { return store[n]; }
    const_reference operator[](size_type n) const { return store[n]; }

    reference       front()       { return store[0]; }
    const_reference front() const { return store[0]; }
    reference       back()        { return store[count-1]; }
    const_reference back()  const { return store[count-1]; }

    const_reference at(size_type n) const { check(n); return store[n]; }
    reference       at(size_type n)       { check(n); return store[n]; }

    // data access:
    T*       data()       { return store; }
    const T* data() const { return store; }
};

} // namespace std

#endif
