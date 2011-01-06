#define major	"beef"
#define M(major)	foo##_##major##_(major)

void foo_30_(int);
void bar(void);

void bar(void)
{
  M(30);
}
