#include "invoke.h"
#define outer(major, minor) inner(major, minor)

void outer(long one, short two) { }
major(three);
minor(four);
