 #include<iostream> 
#include <stdio.h> 
#include <string.h>
#include <cilk/cilk.h> 

using namespace std;

int j[10];
int main2(void);
#if 1
int main(int argc, char **argv)
{
  if (argc == 1)
    main2();

   /* THIS LOOP IS WRONG! I HAVE PURPOSELY KEPT IT HERE 
    * TO SEE IF THE CILK FOR IS WORKING. THE PRINTOUTS
    * SHOULD NOT PRINT CORRECTLY!
    */
   cilk_for (int jj = 0; jj < 10; jj++)  {

     cout << "J[" << jj << "] = " << j[jj] << endl;

   }
   
   cout << endl << endl;

   /* Now, this loop should print correctly */
   for (int jj = 0; jj < 10; jj++)  {
     cout << "J[" << jj << "] = " << j[jj] << endl;
   }

   return 0;
  
}

#endif
  

int main2(void)
{
cilk_for (int ii = 0; ii < 10; ii++)
{
printf("Hello World %d\n", ii); 
j[ii]=ii;
}

return j[9];
}

