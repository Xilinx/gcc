#ifndef X0TEN_HELLOS_H
#define X0TEN_HELLOS_H
extern "C" {
  int printf(const char*, ...);
  void abort(void);
  void exit(int);
};

class A
{
public:
  A() {
    x_ = 0;
    printf ("constructing\n");
  }

  void hello(void) {
    x_++;
    printf ("Hello World (%d)\n", x_);
  }

private:
  int x_;
};
#endif
