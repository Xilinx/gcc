/* This test will create 1 clones of the function below, just mask
 * for the core2_duo with sse 4.1 processor.
 */

#define My_Type float
__attribute__ ((vector(vectorlength(4), processor (core_2_duo_sse_4_1), linear(y), mask, uniform (x))))
My_Type ef_add (My_Type x, My_Type y) 

{
  return x + y;
}

