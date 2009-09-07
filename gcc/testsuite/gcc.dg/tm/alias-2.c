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

void candy ();

struct mystruct_type {
  ptrcast *ptr;
} *mystruct, *mystruct2;

ptrcast *someptr, **pp;
ptrcast ui;

void tootsie_roll () __attribute__((tm_wrap (candy)));
void tootsie_roll ()
{
  ui = TM_LOAD  (&mystruct);
  mystruct2 = (struct mystruct_type *) ui;

  pp = &mystruct2->ptr;
}

void foo()
{
  candy();
}

/* { dg-final { scan-tree-dump-times "ui\..* = same as mystruct" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "mystruct.*ESCAPED" 1 "ealias" } } */
/* { dg-final { scan-tree-dump-times "pp = same as mystruct" 1 "ealias" } } */
/* { dg-final { cleanup-tree-dump "ealias" } } */
