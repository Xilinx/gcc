// Test the fix for a bug in the support of allowing reader locks for
// non-const, non-modifying overload functions. (We didn't handle the builtin
// properly.)
// { dg-do compile }
// { dg-options "-Wthread-safety -O" }

enum MyFlags {
  Zero,
  One,
  Two,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine
};

inline MyFlags
operator|(MyFlags a, MyFlags b)
{
  return MyFlags(static_cast<int>(a) | static_cast<int>(b));
}

inline MyFlags&
operator|=(MyFlags& a, MyFlags b)
{
    return a = a | b;
}
