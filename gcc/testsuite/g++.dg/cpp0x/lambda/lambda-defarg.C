// { dg-options "-std=c++0x" }

int main()
{
  [](int a = 1) { return a; }(); // { dg-message "" }
}
