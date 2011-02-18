#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

static unsigned long long MAX_ITEMS = 10000;

static int
cmpdouble (const void *p1, const void *p2)
{
  double n1 = *((double *)p1);
  double n2 = *((double *)p2);

  if (n1 < n2)
    return -1;
  else if (n1 > n2)
    return 1;
  else
    return 0;
}


double
compute_median (int n, double vec[])
{
  qsort (vec, n, sizeof (double), cmpdouble);
  
  if (n % 2 == 0)
    return ((vec[n / 2] + vec[n / 2 - 1]) / 2.0);
  else
    return vec[n / 2];
}

double
compute_stddev (int n, double avg, double vec[])
{
  double sd, sum, s, x;
  int i;

  for (x = 0.0, sum = 0.0, i = 0; i < n; i++)
    {
      double d;

      x = x + vec[i];
      d = vec[i] - avg;
      sum += d * d;
    }

  s = sum / n;
  sd = sqrt (s);

  return sd;
}


int
main (int argc, char *argv[])
{
  double *vec;
  double x, sum, product, inverse_sum, arithmetic, geometric, harmonic;
  double min, max, median, variance, stddev;
  int count;
  int ret;

  sum = 0.0;
  product = 1.0;
  inverse_sum = 0.0;
  count = 0;
  min = 0.0;
  max = 0.0;

  vec = (double *) malloc (MAX_ITEMS * sizeof (double));

  while (1)
    {
      ret = scanf ("%lg", &x);
      if (ret == EOF)
	break;

      if (count == 0)
	min = max = x;

      if (x < min)
	min = x;

      if (x > max)
	max = x;

      sum += x;
      product *= x;
      inverse_sum += 1.0 / x;
      vec[count] = x;

      count++;
      if (count >= MAX_ITEMS)
	{
	  MAX_ITEMS *= 3;
	  vec = (double *) realloc (vec, MAX_ITEMS * sizeof (double));
	}
    }

  int do_min = (strstr (argv[0], "min") != NULL);
  int do_max = (strstr (argv[0], "max") != NULL);
  int do_avg = (strstr (argv[0], "avg") != NULL);
  int do_geo = (strstr (argv[0], "geoavg") != NULL);
  int do_harmonic = (strstr (argv[0], "harmonic") != NULL);
  int do_median = (strstr (argv[0], "median") != NULL);
  int do_variance = (strstr (argv[0], "variance") != NULL);
  int do_stdev = (strstr (argv[0], "stdev") != NULL);
  int do_all = (argc > 1 && strcmp (argv[1], "-a") == 0);

  if (count > 0)
    {
      arithmetic = sum / count;
      geometric = pow (product, (double) 1.0 / (double) count);
      harmonic = count / inverse_sum;
      median = compute_median (count, vec);
      stddev = compute_stddev (count, arithmetic, vec);
      variance = stddev * stddev;

      if (do_all)
	{
	  printf ("# of items read --> %d\n", count);
	  printf ("Min --------------> %lg\n", min);
	  printf ("Max --------------> %lg\n", max);
	  printf ("Arithmetic mean --> %lg\n", arithmetic);
	  printf ("Geometric mean ---> %lg\n", geometric);
	  printf ("Harmonic mean ----> %lg\n", harmonic);
	  printf ("Median -----------> %lg\n", median);
	  printf ("Variance ---------> %lg\n", variance);
	  printf ("Standard dev -----> %lg\n", stddev);
	}
      else if (do_min)
	printf ("%lg\n", min);
      else if (do_max)
	printf ("%lg\n", max);
      else if (do_avg)
	printf ("%lg\n", arithmetic);
      else if (do_geo)
	printf ("%lg\n", geometric);
      else if (do_harmonic)
	printf ("%lg\n", harmonic);
      else if (do_median)
	printf ("%lg\n", median);
      else if (do_variance)
	printf ("%lg\n", variance);
      else if (do_stdev)
	printf ("%lg\n", stddev);
      else
	{
	  fprintf (stderr, "ERROR: Unknown value '%s' to compute\n", argv[0]);
	  return 1;
	}
    }
  else
    {
      fprintf (stderr, "ERROR: none of the input is positive\n");
      return 1;
    }

  return 0;
}
