// Don't die on local class template arguments
// { dg-options "-std=c++11 -gdwarf-4 -fdwarf-comdat-fn-debug -O" }

template <class T>
void f(T t) { }

template <class T>
void g1(T t)
{
  struct A { } a;
  f(A());
}

template <class T>
inline void g2(T t) __attribute__ ((always_inline));

template <class T>
inline void g2(T t)
{
  struct A { } a;
  f(A());
}

void (*p)(int) = g2<int>;

template <class T>
struct B
{
  static void g3(T);
};

template <class T>
void B<T>::g3(T)
{
  struct A { } a;
  f(A());
}

int main()
{
  g1(42);
  g2(42);
  B<int>::g3(42);
}
