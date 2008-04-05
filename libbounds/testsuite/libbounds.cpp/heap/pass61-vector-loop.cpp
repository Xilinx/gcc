#define SIZE 30

#include <vector>
using namespace std;

class tiny {
public:
  short i;
};

int main()
{
  vector<tiny> *v = new vector<tiny>(SIZE);
  vector<tiny> *w = new vector<tiny>(SIZE);

  for (int j = 0; j < SIZE; j++)
    (*w)[j].i = 0;
  for (int j = 0; j < SIZE; j++)
    (*v)[j].i = 0;

  delete w;
  delete v;

  return 0;
}

