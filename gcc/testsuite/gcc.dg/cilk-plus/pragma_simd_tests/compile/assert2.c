// { dg-do compile }
// { dg-options "-O3" }

void addf(float *a, float *b, float *c, int n)
{
    int i;
#pragma simd assert vectorlength(1) /* { dg-error "vectorlength in pragma" } */
    for (i=0; i<10; i++)
    {
      a[i] = b[i] + c[i];
    }
}
