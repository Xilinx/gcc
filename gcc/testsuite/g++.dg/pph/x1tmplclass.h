#ifndef X1TMPLCLASS_H
#define X1TMPLCLASS_H
template< typename T >
struct wrapper {
  T value;
  static T cache;
};

template< typename T >
T wrapper<T>::cache = 3;

template<>
struct wrapper<char> {
  int value;
  static int cache;
};
#endif
