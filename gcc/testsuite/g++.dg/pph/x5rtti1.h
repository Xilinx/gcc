#ifndef X5RTTI1_H
#define X5RTTI1_H

#include <typeinfo>
#include "a0rtti.h"

const std::type_info& poly1() { return typeid( polymorphic ); }
const std::type_info& nonp1() { return typeid( non_polymorphic ); }
const std::type_info& hpol1() { polymorphic var; return var.hidden(); }
const std::type_info& hnpl1() { non_polymorphic var; return var.hidden(); }

#endif
