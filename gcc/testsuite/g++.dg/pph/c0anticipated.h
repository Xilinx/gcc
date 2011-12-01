#ifndef C0ANTICIPATED_H
#define C0ANTICIPATED_H
extern  "C" {
  typedef __SIZE_TYPE__ size_t;
  void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
  int printf(const char *, ...);
  int atoi(const char *);
}

/* pretty print a board in the specified hexagonal format */
void pretty(char b) {
   int i;
   for(i = 0; i < 1; i += 10) {
      printf("%c ", b + i);
   }
   printf("\n");
}
#endif
