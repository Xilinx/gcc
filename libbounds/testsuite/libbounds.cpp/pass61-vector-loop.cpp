#define SIZE 20

#include <vector>
using namespace std;

class tiny {
public:
  short i;
};

int main()
{
  vector<tiny> v(SIZE);
  vector<tiny> w(SIZE);

  for (int j = 0; j < SIZE; j++)
    w[j].i = 0;
  for (int j = 0; j < SIZE; j++)
    v[j].i = 0;

  return 0;
}

