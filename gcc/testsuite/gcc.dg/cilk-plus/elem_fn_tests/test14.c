/* This test will create 1 clones of the function below, just the mask
 * for the pentium4 with sse3 processor.
 */

#define My_Type float
__attribute__ ((vector(mask, vectorlength(4), processor (core2_duo_sse3), linear(y), uniform (x))))
My_Type ef_add (My_Type x, My_Type y) 

{
  return x + y;
}

