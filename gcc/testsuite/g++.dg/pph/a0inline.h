#ifndef A0INLINE_H
#define A0INLINE_H
struct S { int a, b; };
inline int f(struct S s) { return s.a + s.b; }
#endif
