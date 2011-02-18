#ifndef X1TYPEREFS_H
#define X1TYPEREFS_H
#include "c1typerefs.h"
struct derived : base {
    int method();
};
struct vderived : virtual base {
};
#endif
