/* This test will create 1 clone of the function below, just one for nomask and
   do a linear for y variable
 */

#define My_Type float
__attribute__ ((vector(vectorlength(4), nomask, processor (core_i7_sse4_2), uniform (x), linear(y))))
My_Type ef_add (My_Type x, My_Type y) 

{
  return x + y;
}

