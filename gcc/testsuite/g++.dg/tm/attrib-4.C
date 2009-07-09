// { dg-do compile }
// { dg-options "-fgnu-tm" }

#define __ts	__attribute__((tm_safe))
#define __tc	__attribute__((tm_callable))
#define __tp	__attribute__((tm_pure))
#define __tu	__attribute__((tm_unknown))

struct __ts A
{
  virtual void f();
  virtual void g();
};

struct __tc B : public A
{
  void f() __tc;  // { dg-error ".tm_callable. overriding .tm_safe." }
  void g();
  virtual void h();
};

struct C : public B
{
  void g() __tc;  // { dg-error ".tm_callable. overriding .tm_safe." }
};

struct C2 : public B
{
  void g() __ts;
  void h() __tu;  // { dg-error ".tm_unknown. overriding .tm_callable." }
};

struct D
{
  virtual void f() __tp;
  virtual void g() __tp;
};

struct E : public D
{
  void f() __ts;  // { dg-error ".tm_safe. overriding .tm_pure." }
  void g();
};

struct F : public E
{
  void g() __ts;  // { dg-error ".tm_safe. overriding .tm_pure." }
};
