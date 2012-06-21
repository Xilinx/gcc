int q[10], seq2[10];
int main (int argc, char** argv)
{
   
    int max = 10, start = 0;
      cilk_for(int ii=max - 1; ii>=start; ii--) 
	{ 
	  int jj = 0;
	  while (jj < 10)
	    {
	      seq2[jj] = 1;
	      jj++;
	    }
	  continue;
	}
        return 0;
}

