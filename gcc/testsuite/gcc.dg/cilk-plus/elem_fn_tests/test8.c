/* This test will create 1 clone of the function below, just one for nomask
 * for the pentium4 processor.
 */

#define My_Type float
__attribute__ ((vector(vectorlength(4), nomask, processor (pentium_4), linear(y), uniform (x))))
My_Type ef_add (My_Type x, My_Type y) 

{
  return x + y;
}

