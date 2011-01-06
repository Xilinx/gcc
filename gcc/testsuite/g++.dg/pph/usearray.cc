#include "usearray.h"
const int x = 3;
const int y = 10;
float array[x + y];
float foo (int i)
{
  return array[i * 3];
}
