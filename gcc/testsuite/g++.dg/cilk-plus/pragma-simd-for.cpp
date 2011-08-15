#include <stdio.h> 
#include <stdlib.h>
void foo(int *p);
int d[256], g_a[240];
int some_func(int argc, char**argv) 
{
int a[256], b[256], c[256], x[256];
int return_value = 0;
int i,ii, p=503, v=0,val=0, q=0;
int r = 5;
#if 1
for (ii = 0; ii < 256; ii++)
{
   b[ii] = argc * argc;
   c[ii] = argc + argc;
   d[ii] = argc + argc;
}
#endif
#if 1
#pragma simd assert vectorlength(8,		4, 2) private(p,q)
for (i=0; i<256; i++) {
    p = i+argc;
    q = i+argc*argc;
    a[i] = b[i] + c[i];
    d[i] = p;
    g_a[i] = q;
}
#endif

#if 1
#pragma simd private(r)
for (i=0; i<256; i++) {
    r = i + q;
    d[i] = a[i-1];
    x[i] = r; 
    a[i] = b[i] + c[i];
}
#endif
#if 1
  foo(a); 
#endif
#if 1
   for (ii = 0; ii < 240; ii++)
   {
     printf("%d\n", g_a[ii]);
   } 
#endif
#if  1
#pragma simd reduction(+:v)
  for (ii = 0; ii < 240; ii++) 
  {
    v += (a[ii] - b[ii]) ;
  }
#endif
  return_value = (a[rand()%256] & v);
 

 
return return_value;  
}
#if 1
void foo(int *p) 
{
int i;
#pragma simd assert
for (i = 0; i < 240; i++) {
g_a[i] = *(p+1);
}
}
#endif


#if 1
int main(int argc, char **argv)
{
  int ii = 0;
  some_func(argc, argv);

  for (ii = 0; ii < 256; ii++)
    printf("%d\n", d[ii]);   
  
  return 0;
}
#endif
