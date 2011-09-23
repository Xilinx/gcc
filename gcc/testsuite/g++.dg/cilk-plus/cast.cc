#include <cilk/cilk.h>
#include <cstdio>

struct BruceWillis {
    BruceWillis (int i) : m(i) { }
    ~BruceWillis () { }
    operator int () { return m; }
    BruceWillis operator++ () { ++m; return *this; }
    int m;
};

int main () {
    cilk_for (BruceWillis bw(0);
              bw < (int)BruceWillis(16); // This is the problematic line.
              ++bw);
    std::puts("done.");
    return 0;
}
