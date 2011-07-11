/* { dg-options "-w" }  */

// pph asm xdiff

#include "c2meteor-contest.h"

int main(int argc, char **argv) {
   if(argc > 1)
      max_solutions = atoi(argv[1]);
   calc_pieces();
   calc_rows();
   solve(0, 0);
   printf("%d solutions found\n\n", solution_count);
   qsort(solutions, solution_count, 50 * sizeof(signed char), solution_sort);
   pretty(solutions[0]);
   pretty(solutions[solution_count-1]);
   return 0;
}
