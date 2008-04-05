#include <vector>
using namespace std;

class tiny {
public:
  short i;
};

int main()
{
  vector<tiny> *v = new vector<tiny>(2);
  (*v)[1].i = 4;

  if ((*v)[1].i != 4) return 1;

  delete v;

  return 0;
}

