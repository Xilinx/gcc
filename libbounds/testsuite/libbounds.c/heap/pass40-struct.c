typedef struct {
  int i;
} sTest;

int main()
{
    sTest *p = (sTest *) malloc( sizeof(sTest) );

    p->i = 7;

    if (p->i != 7) return 1;
    return 0;
}

