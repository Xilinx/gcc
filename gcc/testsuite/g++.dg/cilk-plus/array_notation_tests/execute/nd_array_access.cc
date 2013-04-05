#define N 100

int A[N][N][N][N], B[N][N][N][N];

int main (int argc, char **argv) {
    int ii = 0, jj = 0, kk = 0, ll = 0;
  A[:][:][:][:] = argc;
  B[:][:][:][:] = argc * 2;
  
  A[0:N][:][0][0:N] = B[0:N][5][:][0:N:1];

  for (ii = 0; ii < N; ii++)
    for (jj = 0; jj < N; jj++)
      for (kk = 0; kk < N; kk++) 
	{
	  if (A[ii][jj][0][kk] != B[ii][5][jj][kk])
	    return 1;
	}
    
  A[0:N][4][:][0:N] = B[0:N][:][3][0:N:1] + 5;

  for (ii = 0; ii < N; ii++)
    for (jj = 0; jj < N; jj++)
      for (kk = 0; kk < N; kk++) 
	{
	  if (A[ii][4][jj][kk] != B[ii][jj][3][kk] + 5)
	    return 1;
	}
  
  return 0;
}

