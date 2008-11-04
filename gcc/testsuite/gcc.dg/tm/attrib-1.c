/* { dg-do compile } */
/* { dg-options "-fgnu-tm" } */

extern void f1(void) __attribute__((tm_callable));
extern void f2(void) __attribute__((tm_unknown));
extern void f3(void) __attribute__((tm_pure));
extern void f4(void) __attribute__((tm_safe));

extern void g1(void) __attribute__((tm_callable, tm_safe));  /* { dg-error "previously declared" } */

extern int v1 __attribute__((tm_pure));  /* { dg-warning "function types" } */

typedef void t1(void) __attribute__((tm_callable));
typedef void (*t2)(void) __attribute__((tm_callable));
typedef int t3 __attribute__((tm_callable));  /* { dg-warning "function types" } */

typedef void u0(void);
typedef u0 u1 __attribute__((tm_callable));
typedef u1 u2 __attribute__((tm_pure));  /* { dg-error "previously declared" } */
typedef u0 *u3 __attribute__((tm_safe));
typedef u3 u4 __attribute__((tm_unknown));  /* { dg-error "previously declared" } */
