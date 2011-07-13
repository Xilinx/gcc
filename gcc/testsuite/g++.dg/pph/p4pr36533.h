/* PR target/36533 */
/* { dg-options "-w -fpermissive" } */

#ifndef C4PR36533_H
#define C4PR36533_H

#include <sys/mman.h>
#include <string.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#include "a0pr36533.h"

#endif
