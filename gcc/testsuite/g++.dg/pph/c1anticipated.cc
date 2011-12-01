#include "c0anticipated.h"

int max_solutions;
int solution_count;

int main(int argc, char **argv) {
   if(argc > 1)
      max_solutions = atoi(argv[1]);
   printf("%d solutions found\n\n", solution_count);
   pretty('b');
   return 0;
}
