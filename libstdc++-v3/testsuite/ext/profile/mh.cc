#include <stdio.h>
#include <malloc.h>
#include <vector>

using std::vector;

static void my_init_hook (void);
static void *my_malloc_hook (size_t, const void *);
typedef void* (*malloc_hook) (size_t, const void *);

malloc_hook old_malloc_hook;
     
void (*__malloc_initialize_hook) (void) = my_init_hook;

static void
my_init_hook (void)
{
  old_malloc_hook = __malloc_hook;
  __malloc_hook = my_malloc_hook;
}

static void *
my_malloc_hook (size_t size, const void *caller)
{
  void *result;
  __malloc_hook = old_malloc_hook;
  result = malloc (size);
  old_malloc_hook = __malloc_hook;

  // With _GLIBCXX_PROFILE, the instrumentation of the vector constructor
  // will call back into malloc.
  vector<int> v;

  __malloc_hook = my_malloc_hook;
  return result;
}
     

int main() {
  int* test = (int*) malloc(sizeof(int));
  *test = 1;
  return *test;
}
