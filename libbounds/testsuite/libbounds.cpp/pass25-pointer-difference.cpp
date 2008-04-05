int main()
{
	int* p; int* q;
	int size;
	int a[10];

	p = a;
	q = &a[9];
	size = q - p;

	return size - 9;
}
