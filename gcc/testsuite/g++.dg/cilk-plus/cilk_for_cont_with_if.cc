int q[10], seq2[10];
int main (int argc, char** argv)
{
   
    int max = 10, start = 0;
      cilk_for(int ii = max - 1; ii >= start; ii--) 
	{ 
	  if (q[ii] != 0) 
	    continue;
	}
        return 0;
}

