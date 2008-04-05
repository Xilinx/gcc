typedef struct {
  short a[3];
} sTest;

int main()
{
    sTest s;

    s.a[2] = 4;

    if (s.a[2] != 4) return 1;
    return 0;
}

