/* Verify that intermediate coverage format can be generated for simple code. */

/* { dg-options "-fprofile-arcs -ftest-coverage" } */
/* { dg-do run { target native } } */

class C {
public:
  C()
  {
    i = 0;				/* count(1) */
  }
  ~C() {}
  void seti (int j)
  {
    i = j;				/* count(1) */
  }
private:
  int i;
};

void foo()
{
  C c;					/* count(2) */
  c.seti (1);				/* count(1) */
}

int main()
{
  foo();				/* count(1) */
}

/* { dg-final { run-gcov intermediate { -i gcov-7.C } } } */
