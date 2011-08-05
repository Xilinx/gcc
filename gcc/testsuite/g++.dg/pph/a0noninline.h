#ifndef A0NONINLINE_H
#define A0NONINLINE_H
struct S { int a, b; };
int f(struct S s) { return s.a + s.b; }
#endif
