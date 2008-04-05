int main()
{
    int* p;
    int* q;
    int a[10];

    /* a[9] = 0; */
    p = a;
    p = p - 1;
    q = p + 1;
    *q = 0;

    return 0;
}
