// { dg-options -std=c++0x }
// { dg-do run }

extern "C" void abort();

template <class T>
auto apply (T t) -> decltype (t())
{
  return t();
}

template <class T>
T f(T t)
{
  T t2 = t;
  if (t != [=]()->T { return t; }())
    abort ();
  if (t != [=] { return t; }())
    abort ();
  if (t != [=] { return t2; }())
    abort ();
  if (t != [&] { return t; }())
    abort ();
  if (t != apply([=]{return t;}))
    abort ();
}

int main()
{
  f(0xbeef);
}
