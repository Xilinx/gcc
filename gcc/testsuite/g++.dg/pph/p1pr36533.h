/* PR target/36533 */
/* { dg-options "-w -fpermissive" } */

#ifndef C1PR36533_H
#define C1PR36533_H

#include <string.h>
#include <sys/mman.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#include "a0pr36533.h"

#endif
