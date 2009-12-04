// { dg-do "compile" }
// { dg-options "-std=gnu++0x" }

// From N2235

// 1
struct A2
{
  static const int eights = 888;
  static constexpr int nines = 999;
};

A2 a;

// 2
struct pixel
{
  int x, y;
};
constexpr pixel ur = { 1294, 1024 }; // OK

// p4
struct Length
{
   explicit constexpr Length(int i = 0) : val(i) { }
private:
   int val;
};

constexpr int abs(int x)
{ return x < 0 ? -x : x; }    // OK

Length l(abs(-97)); // OK

// p6
class debug_flag
{
public:
   explicit debug_flag(bool);
   constexpr bool is_on(); // error: debug_flag not literal type
private:
   bool flag;
};
