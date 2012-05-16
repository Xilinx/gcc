// PR c++/50258
// { dg-options "-std=c++0x" }

struct Foo {
  static const double d = 3.14; // no warning
};
const double Foo::d;
