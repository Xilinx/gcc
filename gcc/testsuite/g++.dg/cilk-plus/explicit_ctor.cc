#include <cilk/cilk.h>
#include <cstdio>

struct BruceBoxleitner {
    int m;
    BruceBoxleitner (int n = 0) : m(n) { }
    BruceBoxleitner operator--() { --m; return *this; }
};

int operator- (BruceBoxleitner a, BruceBoxleitner b) { return a.m - b.m; }

struct BruceLee {
    int m;
    explicit BruceLee (int n) : m(n) { }
};

bool operator> (BruceBoxleitner a, BruceLee b) { return a.m > b.m; }
int operator- (BruceBoxleitner a, BruceLee b) { return a.m - b.m; }

int main () {
    cilk_for (BruceBoxleitner i = 10; i > BruceLee(0); --i);
    std::puts("done.");
    return 0;
}
