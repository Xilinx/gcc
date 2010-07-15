// { dg-do "compile" }
// { dg-options "-std=gnu++0x" }

// From N2235

// 4.5.3 constant expressions

// p 4
struct A {
  constexpr A(int i) : val(i) { }
  constexpr operator int() { return val; }
  constexpr operator long() { return 43; }
private:
  int val;
};

template<int> struct X { };
constexpr A a = 42;

X<a> x;            // OK: unique conversion to int
int ary[a];        // error: ambiguous conversion

// p 5
struct Z {
  operator int() const { return 42; }
  operator unsigned char() const { return 43; }
};
const Z z = { };
const int n = z; // OK: n is initialized with 42
const long m = z; // error: ambiguous conversion
enum E { v1 = 2, v2 = 10 };
E operator+(E, E);
float array[v1 + v2];   // error: v1+v2 not constant


