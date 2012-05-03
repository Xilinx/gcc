// { dg-options "-std=c++11 -gdwarf-4 -fdwarf-comdat-fn-debug" }

template <class T>
T f(T t)
{
  T ar[t];
  return ar[0];
}

int main()
{
  f(42);
}
