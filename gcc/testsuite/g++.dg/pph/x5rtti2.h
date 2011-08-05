#ifndef X5RTTI2_H
#define X5RTTI2_H

#include <typeinfo>
#include "a0rtti.h"

const std::type_info& poly2() { return typeid( polymorphic ); }
const std::type_info& nonp2() { return typeid( non_polymorphic ); }
const std::type_info& hpol2() { polymorphic var; return var.hidden(); }
const std::type_info& hnpl2() { non_polymorphic var; return var.hidden(); }

#endif
