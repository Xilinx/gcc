typedef struct {
  short a[3];
} sTest;

int main()
{
    sTest *p = (sTest *) malloc( sizeof(sTest) );

    p->a[2] = 4;

	if (p->a[2] != 4) return 1;
    return 0;
}
