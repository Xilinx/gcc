// Test the handling of expressions that depend on template parameters in
// self-assignemnt detection.
// { dg-do compile }
// { dg-options "-Wself-assign" }

template<typename T>
struct Bar {
  T x;
  Bar operator++(int) {
    Bar tmp = *this;
    ++x;
    tmp = tmp; // { dg-warning "assigned to itself" }
    return tmp;
  }
};

template<typename T>
T DoSomething(T y) {
  T a[5], *p;
  Bar<T> b;
  b.x = b.x;
  *p = *p;
  a[2] = a[2];
  return *p;
}

main() {
  Bar<int> bar;
  bar++;
  DoSomething(5);
}
