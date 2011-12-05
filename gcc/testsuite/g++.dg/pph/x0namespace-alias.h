#ifndef X0NAMESPACE_ALIAS_H
#define X0NAMESPACE_ALIAS_H

namespace foo {
    class C1;
    class C2;
    struct S1;
};

namespace bar = foo;

namespace foo {
  class C2 {
    public:
      int fld;
  };
};

int f(bar::C2 c);

#endif  // X0NAMESPACE_ALIAS_H
