/* { dg-options "-Winvalid-pph" }  */
/* We do not support #include_next in PPH images.  */
#ifndef D0INCLUDE_NEXT_H
#define D0INCLUDE_NEXT_H
#include_next "d0include-next.h" // { dg-warning ".*PPH generation disabled" }
#endif
