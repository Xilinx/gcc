#define SIZE 100

int main()
{
	int a[SIZE];
	int b[SIZE];

    for (int *p = &b[0]; p < &b[SIZE]; p++)
		*p = 0;
    for (int *p = &a[0]; p < &a[SIZE]; p++)
		*p = 0;

	return 0;
}
