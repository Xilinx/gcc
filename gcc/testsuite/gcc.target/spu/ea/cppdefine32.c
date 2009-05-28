/* Test default __EA32__/__EA64__ define.  */
/* { dg-options "-std=gnu89 -pedantic-errors -mea32" } */
/* { dg-do compile } */

#if !defined (__EA32__) && !defined (__EA64__)
#error both __EA32__ and __EA64__ undefined
#endif

#if defined (__EA32__) && defined (__EA64__)
#error both __EA32__ and __EA64__ defined
#endif

#ifdef __EA32__
int x [ sizeof (__ea char *) == 4 ? 1 : -1 ];
#endif

#ifdef __EA64__
int x [ sizeof (__ea char *) == 8 ? 1 : -1 ];
#endif

