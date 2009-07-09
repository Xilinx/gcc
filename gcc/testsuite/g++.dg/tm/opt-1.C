// { dg-do compile }
// { dg-options "-fgnu-tm -O" }

struct S
{
  virtual void f() __attribute__((tm_safe));
};

void f(S *s) { __tm_atomic { s->f(); } }
