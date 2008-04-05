#define SIZE 100

int main ()
{
	int *a; int *b;

	a = new int[SIZE];
	b = new int[SIZE];

    for (int* p = &a[0]; p < &a[SIZE]; p++)
		*p = 0;
    for (int* p = &b[0]; p < &b[SIZE]; p++)
		*p = 0;

	delete a;
	delete b;

	return 0;
}
