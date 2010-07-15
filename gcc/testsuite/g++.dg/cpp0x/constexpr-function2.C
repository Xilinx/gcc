// { dg-do "compile" }
// { dg-options "-std=gnu++0x" }

// From N2235

// Mess with the builtin by redeclaring.
constexpr int abs(int x) { return x < 0 ? -x : x; }

extern "C"
{
  constexpr float
  squaref(float x) { return x * x; }
}

// implicitly inline, already: warn?
inline constexpr double
squared(double x) { return x * x; }

constexpr int squarei(int x) { return x * x; }
extern const int side;
constexpr int area = squarei(side); // error: squarei(side) is not a
				    //   constant expression

int next(constexpr int x) // error: argument
{ return x + 1; }

constexpr void f(int x)       // error: return type is void
{ /* ... */ }

constexpr int prev(int x)
{ return --x; }               // error: use of decrement

constexpr int g(int x, int n) // error: body not just ‘‘return expr’’
{
   int r = 1;
   while (--n > 0) r *= x;
   return r;
}

constexpr int
bar(int x, int y) { return x + y + x * y; }

int bar(int x, int y)      // error: redefinition of bar
{ return x * 2 + 3 * y; }

constexpr int twice(int x);
enum { bufsz = twice(256) };    // error: twice() isn’t (yet) defined

constexpr int fac(int x)
{ return x > 2 ? x * fac(x - 1) : 1; } // error: fac() not defined
					// before use
