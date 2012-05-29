/* This test will create 1 clones of the function below, just mask 
 * for the pentium4 processor.
 */

#define My_Type float
__attribute__ ((vector(vectorlength(4), mask, processor (pentium_4_sse3), linear(y), uniform (x))))
My_Type ef_add (My_Type x, My_Type y) 

{
  return x + y;
}

