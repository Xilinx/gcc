#ifndef X0DEPENDENT_H
#define X0DEPENDENT_H

template<typename T>
struct outer;

template<>
struct outer< void >
{
  template< typename U >
  struct inner
  {
    typedef outer<U> other;
  };
};

template< typename T >
struct outer
{
  template< typename U >
  struct inner
  {
    typedef outer<U> other;
  };
};

#endif
