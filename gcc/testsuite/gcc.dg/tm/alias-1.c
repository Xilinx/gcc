/* { dg-do compile } */
/* { dg-options "-fgnu-tm -fdump-tree-ealias -O" } */

#if (__SIZEOF_LONG_LONG__ == __SIZEOF_POINTER__)
typedef unsigned long long ptrcast;
#elif (__SIZEOF_LONG__ == __SIZEOF_POINTER__)
typedef unsigned long ptrcast;
#elif (__SIZEOF_INT__ == __SIZEOF_POINTER__)
typedef unsigned int ptrcast;
#else
#error Add target support here
#endif

#if (__SIZEOF_POINTER__ == 4)
#define TM_LOAD  __builtin__ITM_RU4
#define TM_STORE __builtin__ITM_WU4
#elif (__SIZEOF_POINTER__ == 8)
#define TM_LOAD __builtin__ITM_RU8
#define TM_STORE __builtin__ITM_WU8
#else
#error Add target support here
#endif

struct mystruct_type {
  ptrcast *ptr;
} *mystruct;

ptrcast *someptr, **pp;
ptrcast ui;

void f(void)
{
  __tm_atomic {
    ui = TM_LOAD  (&mystruct);
    mystruct = (struct mystruct_type *) ui;
    ui = TM_LOAD  (&someptr);
    someptr = (ptrcast *) ui;
    ui = (ptrcast) someptr;
    pp = &mystruct->ptr;
    TM_STORE (pp, ui);
  }
}

/* { dg-final { scan-tree-dump-times "mystruct = \{ .*ESCAPED" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "someptr = same as mystruct" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "ui\..* = same as mystruct" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "pp\..* = same as mystruct" 1 "ealias" } } */
/* { dg-final { cleanup-tree-dump "ealias" } } */
