/* { dg-do compile } */
/* { dg-options "-fgnu-tm" } */

extern void f1(void) __attribute__((transaction_callable));
extern void f2(void) __attribute__((transaction_unsafe));
extern void f3(void) __attribute__((transaction_pure));
extern void f4(void) __attribute__((transaction_safe));

extern void g1(void) __attribute__((transaction_callable, transaction_safe));  /* { dg-error "previously declared" } */

extern int v1 __attribute__((transaction_pure));  /* { dg-warning "ignored" } */

typedef void t1(void) __attribute__((transaction_callable));
typedef void (*t2)(void) __attribute__((transaction_callable));
typedef int t3 __attribute__((transaction_callable));  /* { dg-warning "ignored" } */

typedef void u0(void);
typedef u0 u1 __attribute__((transaction_callable));
typedef u1 u2 __attribute__((transaction_pure));  /* { dg-error "previously declared" } */
typedef u0 *u3 __attribute__((transaction_safe));
typedef u3 u4 __attribute__((transaction_unsafe));  /* { dg-error "previously declared" } */
