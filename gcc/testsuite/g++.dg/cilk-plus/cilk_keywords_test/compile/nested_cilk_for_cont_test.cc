int q[10], seq2[10];
int main (int argc, char** argv)
{
   
    int max = 10, start = 0;
      cilk_for(int ii=max - 1; ii>=start; ii--) 
	{ 
	  cilk_for (int jj = 0; jj < 10; jj++)
	    {
	      if (seq2[jj] == 5)
		continue; /* This continue should point to a different place
			     than the one below */
	      else
		seq2[jj] = 2;
	    }
	  continue;
	}
        return 0;
}

