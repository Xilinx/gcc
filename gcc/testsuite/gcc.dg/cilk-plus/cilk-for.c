#include <stdio.h> 
#include <string.h>
#include <cilk/cilk.h> 


int j[10];
int main2(void);

int main(int argc, char **argv)
{
  int jj = 0;
  char q[11] = {'H','e','l','l','o','W','o','r','l','d','\0'};
  if (argc == 1)
    main2();

#if 1
   /* THIS LOOP IS WRONG! I HAVE PURPOSELY KEPT IT HERE 
    * TO SEE IF THE CILK FOR IS WORKING. THE PRINTOUTS
    * SHOULD NOT PRINT CORRECTLY!
    */
   cilk_for (jj = 0; jj < 10; jj++)  {
     printf("J[%2d] = %2d\n", jj, j[jj]);
     printf("%c\t", q[jj]);
     fflush(stdout);
   }
   

   /* Now, this loop should print correctly */
   for (jj = 0; jj < 10; jj++)  {
     printf("J[%2d] = %2d\n", jj, j[jj]);
     printf("%c\t", q[jj]);
   }
#endif
   return 0;
  
}

 
  

int main2(void)
{
int ii ;
int w ;
char q[11] = {'H','e','l','l','o','W','o','r','l','d','\0'};

w = 5; 
cilk_for (ii = 0; ii < 10; ii++)
{
printf("Hello World %d\n", ii); 
j[ii]=ii;
printf("q[%d] = %c\n",ii,q[ii]);
}

return j[9];
}

