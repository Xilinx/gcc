int main()
{
	int* p; int* q;
	int size;

	p = new int[10];
	q = &p[10];
	size = q - p;

	delete p;

	return size - 10;
}
