#include <stdio.h> 
#include <string.h> 
#include <cilk/cilk.h> 


int main(int argc, char**argv) 
{
  cilk_for (int ii = 0; ii < 10; ii++) { 
    cilk_for (int jj = 0; jj < 10; jj++) { 
      cilk_for (int kk1 = 0; kk1 < 10; kk1++) {
	cilk_for (int kk2 = 0; kk2 < 10; kk2++) {
	  cilk_for (int kk3 = 0; kk3 < 10; kk3++) {
	    cilk_for (int kk4 = 0; kk4 < 10; kk4++) {
	      cilk_for (int kk5 = 0; kk5 < 10; kk5++) {
		cilk_for (int kk6 = 0; kk6 < 10; kk6++) {
		  cilk_for (int kk7 = 0; kk7 < 10; kk7++) {
		    cilk_for (int kk8 = 0; kk8 < 10; kk8++) {
		      cilk_for (int kk9 = 0; kk9 < 10; kk9++) {
			cilk_for (int kk10 = 0; kk10 < 10; kk10++) {
			  cilk_for (int kk11 = 0; kk11 < 10; kk11++) {
			    printf("Hello World %d!\n",
                                   ii+jj+kk1+kk2+kk3+kk4+kk5+kk6+kk7+kk8+kk9+kk10+kk11);
			  }
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }

  return 0;  
}

