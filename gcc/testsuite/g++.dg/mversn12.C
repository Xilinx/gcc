/* Check if everything is fine if the versioned functions are static
   member functions. */

/* { dg-do run } */
/* { dg-options "-O2 -fclone-hot-version-paths" } */

int  __attribute__ ((version_selector))
featureTest()
{
  return 1;;
}

class TestClass
{
 public:
  static int foo ()
  {
    return 0;
  }

  static int bar ()
  {
    return 1;
  }

  int dispatch ()
  {
    int a = __builtin_dispatch (featureTest,
                               (void*)(&TestClass::foo),
			       (void*)(&TestClass::bar));
    int b = __builtin_dispatch (featureTest,
                               (void*)(&TestClass::bar),
			       (void*)(&TestClass::foo));
    return a * b;
  }
};

int
main ()
{
  TestClass c1;
  return c1.dispatch ();
}
