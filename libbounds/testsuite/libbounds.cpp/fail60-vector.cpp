#include <vector>
using namespace std;

class tiny {
public:
  short i;
};

int main()
{
  vector<tiny> v(2);
  v[2].i = 4;

  return 0;
}
/* { dg-output "bounds violation.*" } */
/* { dg-do run { xfail *-*-* } } */
