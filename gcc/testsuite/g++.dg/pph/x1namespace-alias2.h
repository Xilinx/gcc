#ifndef X1NAMESPACE_ALIAS2_H
#define X1NAMESPACE_ALIAS2_H
#include "x0namespace-alias.h"
namespace foo {
  class C1 {
    public:
      int another_fld;
  };
};
int h(bar::C1 c);
#endif
