int q[10], seq[10];

template <typename T>
T my_func  (T max ,T start) 
{   
      cilk_for (T ii = max - 1; ii >= start; ii--) 
	{ 
	  if (ii % 2) 
	    continue;  /* this continue should jump to a different place than
			  the one below */
	  for (int jj = 0; jj < 10; jj++)  
	    {
	      if (seq[jj] == 5)
		continue;
	      else
		seq[jj] = 2;
	    }
	}
      return 0;
}

int main (int argc, char** argv)
{
  my_func <int> (10, 0);
  my_func <char> (10, 0);
  return 0;
}
