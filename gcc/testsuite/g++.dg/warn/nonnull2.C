// Test that "nonnull" attribute should not be applied to 'this' pointer.
// { dg-do compile }

#define NULL 0

class Foo
{
public:
  void method1 (const int *ptr) __attribute__ ((nonnull (1, 2)));	// { dg-warning "nonnull argument references 'this' pointer" }
  void method2 (int *ptr1, int a, int *ptr2) __attribute__ ((nonnull (2, 3, 4)));	// { dg-error "nonnull argument references non-pointer operand" }
  static void func3 (int *ptr) __attribute__ ((nonnull (1)));	// should not warn
    Foo (char *str) __attribute__ ((nonnull ()))
  {
  }
};

int func4 (int *ptr1, int a) __attribute__ ((nonnull (1)));	// should not warn
