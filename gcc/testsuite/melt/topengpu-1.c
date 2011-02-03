/* file gcc/testsuite/melt/topengpu-1.c; inspired from file
   matrix/oper.c of the gsl library version 1.14
   http://www.gnu.org/software/gsl/ */

/* run in $GCCMELT_BUILD/gcc 
  ln -s $GCCMELT_SOURCE/gcc/testsuite/melt/topengpu-1.c .
 ./cc1 -fmelt-mode=opengpu -fmelt-module-path=melt-modules -fmelt-source-path=melt-sources -fmelt-debug -O2 -fgraphite -floop-parallelize-all  topengpu-1.c
  ## it could be useful to pass -fdump-tree-all -fdump-ipa-all
*/

#include <stdlib.h>

struct gsl_block_struct
{
  size_t size;
  double *data;
};

typedef struct gsl_block_struct gsl_block;

typedef struct
{
  size_t size1;
  size_t size2;
  size_t tda;
  double *data;
  gsl_block *block;
  int owner;
} gsl_matrix;

typedef struct
{
  gsl_matrix matrix;
} _gsl_matrix_view;


extern void say_error (const char *msg);


int
gsl_matrix_add (gsl_matrix * a, const gsl_matrix * b)
{
  const size_t M = a->size1;
  const size_t N = a->size2;

  if (b->size1 != M || b->size2 != N)
    {
      say_error ("matrices must have same dimensions");
      return -1;
    }
  else
    {
      const size_t tda_a = a->tda;
      const size_t tda_b = b->tda;

      size_t i, j;

      for (i = 0; i < M; i++)
	{
	  for (j = 0; j < N; j++)
	    {
	      a->data[i * tda_a + j] += b->data[i * tda_b + j];
	    }
	}

      return 0;
    }
}

int
gsl_matrix_sub (gsl_matrix * a, const gsl_matrix * b)
{
  const size_t M = a->size1;
  const size_t N = a->size2;

  if (b->size1 != M || b->size2 != N)
    {
      say_error ("matrices must have same dimensions");
      return -1;
    }
  else
    {
      const size_t tda_a = a->tda;
      const size_t tda_b = b->tda;

      size_t i, j;

      for (i = 0; i < M; i++)
	{
	  for (j = 0; j < N; j++)
	    {
	      a->data[i * tda_a + j] -= b->data[i * tda_b + j];
	    }
	}

      return 0;
    }
}
