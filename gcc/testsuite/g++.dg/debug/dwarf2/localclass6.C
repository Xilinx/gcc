// { dg-options "-std=c++11 -gdwarf-4 -fdwarf-comdat-fn-debug" }

template <class T>
void f(T) { }

template <class T>
void g(T)
{
  struct A { };
  struct B { };
  B (*p)(A) = 0;
  f(p);
}

int main()
{
  g(42);
}
