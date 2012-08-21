/* { dg-do run } */
/* { dg-options "-ldl -lcilkrts" } */

#include <cilk/cilk.h>
#include <cstdlib>

class some_class
{
public:
  some_class(int y);
  ~some_class();
  void print_x();
  void set_x_of_ii_to_value (int index, int value);
  void set_x_of_ii_to_value_serial (int index, int value);
private:
  int x[5];
  int x_serial[5];
};

some_class::some_class(int y)
{
  cilk_spawn set_x_of_ii_to_value (0, y);
  cilk_spawn set_x_of_ii_to_value (1, y);
  cilk_spawn set_x_of_ii_to_value (2, y);
  cilk_spawn set_x_of_ii_to_value (3, y);
  cilk_spawn set_x_of_ii_to_value (4, y);
  set_x_of_ii_to_value_serial (0, y);
  set_x_of_ii_to_value_serial (1, y);
  set_x_of_ii_to_value_serial (2, y);
  set_x_of_ii_to_value_serial (3, y);
  set_x_of_ii_to_value_serial (4, y);
}

some_class::~some_class()
{
  cilk_spawn set_x_of_ii_to_value (0, 0);
  cilk_spawn set_x_of_ii_to_value (1, 0);
  cilk_spawn set_x_of_ii_to_value (2, 0);
  cilk_spawn set_x_of_ii_to_value (3, 0);
  cilk_spawn set_x_of_ii_to_value (4, 0);
  set_x_of_ii_to_value_serial (0, 0);
  set_x_of_ii_to_value_serial (1, 0);
  set_x_of_ii_to_value_serial (2, 0);
  set_x_of_ii_to_value_serial (3, 0);
  set_x_of_ii_to_value_serial (4, 0);
}

void
some_class::set_x_of_ii_to_value (int index, int value)
{
  x[index] = value;
}
void
some_class::set_x_of_ii_to_value_serial (int index, int value)
{
  x_serial[index] = value;
}

void
some_class::print_x(void)
{
  for (int ii = 0; ii < 5; ii++)
    if (x[ii] != x_serial[ii])
      abort ();
  return;
}

int main(int argc, char **argv)
{
  some_class sc(523 + argc); 
  sc.print_x();

  return 0;
}
