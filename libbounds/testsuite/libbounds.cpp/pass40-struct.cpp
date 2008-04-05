typedef struct {
  int i;
} sTest;

int main()
{
    sTest s;

    s.i = 7;

    if (s.i != 7) return 1;
    return 0;
}

