#ifndef D0SDYMNOTINC_H
#define D0SDYMNOTINC_H
struct T { int f; };
struct T NAME = { VALUE }; // { dg-error "was not declared in this scope" }
#endif
