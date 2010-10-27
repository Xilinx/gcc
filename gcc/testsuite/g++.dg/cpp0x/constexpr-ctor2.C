// { dg-do compile }

struct A
{
  A();
};

struct B : A
{
  constexpr B(): A() { }	// { dg-error "A::A" }
};
