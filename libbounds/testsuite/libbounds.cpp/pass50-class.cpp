class sTest {
public:
  int i;
};

int main()
{
    sTest s;

    s.i = 7;

    if (s.i != 7) return 1;
    return 0;
}

